#include "Rendering/Renderer.h"
#include "Math/Vectors.h"
#include "Scene/DistantLight.h"
#include "Scene/SpotLight.h"
#include "Scene/PointLight.h"
#include "Rendering/BiasManager.h"
#include "UI/HudOverlay.h"




Renderer::Renderer(
    std::shared_ptr<QImage> img,
    std::shared_ptr<Scene> scene,
    std::shared_ptr<Camera> camera) :
    renderingSurface(std::make_shared<RenderingSurface>(img)) ,
    scene(scene),
    camera(camera),
    zBuffer(std::make_shared<Buffer<double>>(img->width(), img->height(), std::numeric_limits<double>::infinity())),
    idBuffer(std::make_shared<Buffer<IdBufferElement>>(img->width(), img->height(), IdBufferElement{})),
    paintTool(img, zBuffer)
{
    pixelPainter = std::make_shared<PixelPainter>(renderingSurface->getImg());
    clippingManager = std::make_shared<ClippingManager>();
    lightingManager = std::make_shared<LightingManager>();
}

void Renderer::renderScene(){
    clearBuffers();
    updateCommonMatrices();
    updateShadowMaps();
    renderSceneObjects();

}

void Renderer::renderSceneObjects(){


    stats.clear();

    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){
        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){

            renderObject(*curObject , objIt);
        }
    }
}

/* --+-- Lasciate ogni speranza, voi ch'entrate --+-- */
void Renderer::renderObject(RenderableObject3D& obj, int objId){


    //object is hidden => return
    if(obj.displaySettings.get()->renderMode == DisplaySettings::RenderMode::NONE){
        return;
    }

    // preprocess
    const int width  = getRenderingSurface()->getImg()->width();
    const int height = getRenderingSurface()->getImg()->height();
    const Matrix4 M = obj.transform.getTransMatrix();
    const Color baseColor = obj.viewportDisplay.color;
    Color finalColor = baseColor;
    bool objDrawn = false; //whether there was any pixel of this object drawn on scene, used in statistics
    const Material& mat = obj.material;

    //building clip-space for all vertices and depending on shading, getting color on vertices
    std::vector<ClippingManager::ClippedVertex> clipVertices;
    for(size_t i = 0; i < obj.vertices.size(); i++){

        const Vector3& localSpaceVertex = obj.vertices[i];

        Vector4 clipSpaceVertex = modelToClip(localSpaceVertex , M);
        double invertedW = 1.0/clipSpaceVertex.w;
        Vector3 worldSpaceVertex = modelToWorld(localSpaceVertex, M);
        Vector3 worldSpaceNormal = normalToWorld(obj.transform , obj.vertexNormals[i]);


        Vector2 uv{};
        bool hasUV = false;
        if (i < obj.textureCoords.size() && obj.vertexHasUV[i]) {
            uv = obj.textureCoords[i];
            hasUV = true;
        }

        clipVertices.push_back({
            clipSpaceVertex,
            invertedW,
            worldSpaceVertex * invertedW,
            worldSpaceNormal * invertedW,
            Vector3{}, //not used, should be removed in future
            uv*invertedW,
            hasUV
        });
    }

    //iterate through every face
    for (size_t face = 0; face < obj.faceVertexIndices.size(); face += 3)
    {

        //original vertices in clip-space
        Triangle<ClippingManager::ClippedVertex> clipSpaceTriangle(clipVertices[obj.faceVertexIndices[face    ]],
                                                               clipVertices[obj.faceVertexIndices[face + 1]],
                                                               clipVertices[obj.faceVertexIndices[face + 2]]);

        if(obj.displaySettings->backFaceCulling && shouldCullBackFace( Triangle3(modelToWorld(obj.vertices[obj.faceVertexIndices[face]],M),
                                                                                modelToWorld(obj.vertices[obj.faceVertexIndices[face+1]],M),
                                                                                modelToWorld(obj.vertices[obj.faceVertexIndices[face+2]],M) ))) continue;

        //clipping triangle
        std::vector<ClippingManager::ClippedVertex> clippedPoly = clippingManager->clipTriangle({clipSpaceTriangle.v1,
                                                                                                 clipSpaceTriangle.v2,
                                                                                                 clipSpaceTriangle.v3});
        if (clippedPoly.size() < 3) continue;

        // clip -> ndc -> screen - > screen with normalized depth
        std::vector<Vector3> screenVerticesWithDepth;
        screenVerticesWithDepth.reserve(clippedPoly.size());
        for (ClippingManager::ClippedVertex& cv : clippedPoly){
            Vector3 normalizedClipVertex = clipToNdc(cv.clip);
            Vector2 screenSpaceVertex = ndcToScreen(normalizedClipVertex);
            screenVerticesWithDepth.push_back(Vector3(screenSpaceVertex.x , screenSpaceVertex.y, ((cv.clip.z/cv.clip.w)+1.0)*0.5));
        }

        //Fill-pass
        for (size_t k = 1; k + 1 < clippedPoly.size(); k++)
        {

            bool triDrew = false; //whether any pixel in this triangle was drawn

            // Fan-triangulation
            Triangle3 fanTriangleScreenSpace(screenVerticesWithDepth[0],
                                      screenVerticesWithDepth[k],
                                      screenVerticesWithDepth[k+1]);

            Triangle<ClippingManager::ClippedVertex> fanTriangleClipped(clippedPoly[0],
                                                                        clippedPoly[k],
                                                                        clippedPoly[k+1]);

            // bounding box for rasterization
            // beyond this rectangle there's no need to make further checks
            auto [minX, maxX] = fanTriangleScreenSpace.getBorderX(0 , width);
            auto [minY, maxY] = fanTriangleScreenSpace.getBorderY(0 , height);

            //calculating area of triangle
            //if triangle's area = 0, nothing to raster
            //inv area will be used in further calculations
            double area = fanTriangleScreenSpace.area();
            if(area == 0.0) continue;

            //calculating raw world coordinates for dynamic bias calculations
            //yes it's ugly, probably should just store raw parameters in ClippedVertex
            Triangle3 fanTriangleRawWorldSpace((fanTriangleClipped.v1.worldSpaceVertexOverW)/(fanTriangleClipped.v1.invW),
                                               fanTriangleClipped.v2.worldSpaceVertexOverW/fanTriangleClipped.v2.invW,
                                               fanTriangleClipped.v3.worldSpaceVertexOverW/fanTriangleClipped.v3.invW);


            GouraudShadingFaceData gouraudShadingFaceData;
            FlatShadingFaceData flatShadingFaceData;
            switch(obj.displaySettings->shadingMode){
                case DisplaySettings::Shading::FLAT:
                    flatShadingFaceData = collectFlatPerFaceData(fanTriangleRawWorldSpace, obj);
                    break;
                case DisplaySettings::Shading::GOURAUD:
                    gouraudShadingFaceData = collectGouraudPerFaceData(
                        fanTriangleClipped, fanTriangleRawWorldSpace,obj);
                    break;
                case DisplaySettings::Shading::PHONG:
                    break;
                default:
                    break;
            }

            // raster fill
            if(obj.displaySettings->renderMode == DisplaySettings::RenderMode::RASTER){
                for (int y = minY; y <= maxY; y++) {
                    for (int x = minX; x <= maxX; x++) {

                        double px = x + 0.5, py = y + 0.5; //middle of current pixel
                        Triangle<double> baricentricFactor = fanTriangleScreenSpace.baricentricCoords(Vector2(px, py));

                        //checking if pixel is withing triangle using baricentric coordinates
                        if(!fanTriangleScreenSpace.isInTriangle2D(Vector2(px,py))) continue;

                        double depth = (baricentricFactor.v1*fanTriangleClipped.v1.clip.z/fanTriangleClipped.v1.clip.w
                                        + baricentricFactor.v2*fanTriangleClipped.v2.clip.z/fanTriangleClipped.v2.clip.w
                                        + baricentricFactor.v3*fanTriangleClipped.v3.clip.z/fanTriangleClipped.v3.clip.w)
                                       /
                                       (baricentricFactor.v1/fanTriangleClipped.v1.clip.w
                                          + baricentricFactor.v2/fanTriangleClipped.v2.clip.w
                                          + baricentricFactor.v3/fanTriangleClipped.v3.clip.w);
                        depth = depth*0.5 + 0.5;

                        double invDenom = baricentricFactor.v1 * fanTriangleClipped.v1.invW
                                          + baricentricFactor.v2 * fanTriangleClipped.v2.invW
                                          + baricentricFactor.v3 * fanTriangleClipped.v3.invW;

                        //Material and texture elements preperations
                        bool triangleHasUV = (fanTriangleClipped.v1.hasUV && fanTriangleClipped.v2.hasUV && fanTriangleClipped.v3.hasUV);
                        Vector2 pointUV{};
                        if(triangleHasUV){
                            Vector2 uvOverW = fanTriangleClipped.v1.uvOverW * baricentricFactor.v1
                                              + fanTriangleClipped.v2.uvOverW * baricentricFactor.v2
                                              + fanTriangleClipped.v3.uvOverW * baricentricFactor.v3;
                            pointUV = uvOverW / invDenom;
                        }
                        Vector3 kdTex;
                        Vector3 kd;
                        if(triangleHasUV && obj.displaySettings->rasterMode == DisplaySettings::RasterMode::TEXTURE){
                            kdTex = Texture::sampleRGB(mat.albedoTexture,   pointUV);
                        }else{
                            kdTex = Vectors::colorToVector3(obj.viewportDisplay.color);
                        }
                        kd = Vector3{mat.Kd.x * kdTex.x, mat.Kd.y * kdTex.y, mat.Kd.z * kdTex.z};
                        Vector3 ksTex = triangleHasUV ? Texture::sampleRGB(mat.specularTexture, pointUV) : Vector3{1,1,1};
                        Vector3 ks = Vector3{mat.Ks.x * ksTex.x, mat.Ks.y * ksTex.y , mat.Ks.z * ksTex.z};
                        double  alpha = triangleHasUV ? Texture::sampleA  (mat.opacityTexture,  pointUV) : mat.d;
                        Vector3 keTex = triangleHasUV ? Texture::sampleRGB(mat.emissiveTexture, pointUV) : Vector3{1,1,1};
                        Vector3 Ke = Vector3{mat.Ke.x * keTex.x, mat.Ke.y * keTex.y, mat.Ke.z * keTex.z};
                        Vector3 Ka = mat.Ka;
                        if (alpha < 0.5){
                            continue;
                        }

                        const double SHADOW_INTENSITY = 0.2;
                        Vector3 outColor = Ke; //we start with emission because it's not dependant on the shadow

                        Vector3 ambient = lightingManager->getConstantAmbient(kd,Ka,scene->ambientColor,scene->ambientIntensity,scene->ambientPBR,obj.material.metallic);
                        outColor = outColor + ambient;

                        size_t lightId = 0;
                        for(std::shared_ptr<Light>&lightSource : scene->lightSources){

                            Vector3 shadowTintModifier{}, diffuseModifier{}, specularModifier{}, combinedLight{};
                            Vector3 worldSpacePoint, faceNormal;
                            double shadowAmount{}, visibility{};

                            switch (obj.displaySettings->shadingMode){
                                case DisplaySettings::Shading::FLAT:{

                                    worldSpacePoint = flatShadingFaceData.flatWorldCentroid;
                                    faceNormal = flatShadingFaceData.flatFaceNormal;

                                    shadowAmount = getShadowAmount(*lightSource, worldSpacePoint, faceNormal, fanTriangleRawWorldSpace);
                                    visibility = getVisibility(shadowAmount);

                                    combinedLight = flatShadingFaceData.combinedLight[lightId];
                                    diffuseModifier = flatShadingFaceData.diffuseNoAlbedo[lightId].hadamard(kd)*visibility;
                                    specularModifier = flatShadingFaceData.specularWithLight[lightId]*visibility;
                                    shadowTintModifier = getShadowTintModifier(kd, combinedLight, SHADOW_INTENSITY, shadowAmount);

                                    break;
                                }
                                case DisplaySettings::Shading::GOURAUD:{

                                    worldSpacePoint = (fanTriangleClipped.v1.worldSpaceVertexOverW*baricentricFactor.v1
                                                                    + fanTriangleClipped.v2.worldSpaceVertexOverW*baricentricFactor.v2
                                                                    + fanTriangleClipped.v3.worldSpaceVertexOverW*baricentricFactor.v3)/invDenom;
                                    faceNormal = gouraudShadingFaceData.faceNormal;

                                    shadowAmount = getShadowAmount(*lightSource, worldSpacePoint, faceNormal, fanTriangleRawWorldSpace);
                                    visibility = getVisibility(shadowAmount);

                                    Triangle3 gouraudCombLightOverW = {gouraudShadingFaceData.face.v1.combinedLightOverW[lightId],
                                                                       gouraudShadingFaceData.face.v2.combinedLightOverW[lightId],
                                                                       gouraudShadingFaceData.face.v3.combinedLightOverW[lightId]};

                                    Triangle3 gouraudDiffuseOverW = {gouraudShadingFaceData.face.v1.diffuseNoAlbedoOverW[lightId],
                                                                     gouraudShadingFaceData.face.v2.diffuseNoAlbedoOverW[lightId],
                                                                     gouraudShadingFaceData.face.v3.diffuseNoAlbedoOverW[lightId]};

                                    Triangle3 gouraudSpecularOverW = {gouraudShadingFaceData.face.v1.specularOverW[lightId],
                                                                      gouraudShadingFaceData.face.v2.specularOverW[lightId],
                                                                      gouraudShadingFaceData.face.v3.specularOverW[lightId]};

                                    combinedLight = interpolateCombinedLight(gouraudCombLightOverW, baricentricFactor, invDenom);
                                    shadowTintModifier =getShadowTintModifier(kd, combinedLight, SHADOW_INTENSITY, shadowAmount);
                                    diffuseModifier = interpolateDiffuseModifier(gouraudDiffuseOverW, baricentricFactor, kd, invDenom, visibility);
                                    specularModifier = interpolatSpecularModifier(gouraudSpecularOverW, baricentricFactor, invDenom, visibility);

                                    break;
                                }
                                case DisplaySettings::Shading::PHONG:{
                                    worldSpacePoint = (fanTriangleClipped.v1.worldSpaceVertexOverW*baricentricFactor.v1
                                                                    + fanTriangleClipped.v2.worldSpaceVertexOverW*baricentricFactor.v2
                                                                    + fanTriangleClipped.v3.worldSpaceVertexOverW*baricentricFactor.v3)/invDenom;

                                    faceNormal = ((fanTriangleClipped.v1.worldSpaceNormalOverW*baricentricFactor.v1
                                                                         + fanTriangleClipped.v2.worldSpaceNormalOverW*baricentricFactor.v2
                                                                         + fanTriangleClipped.v3.worldSpaceNormalOverW*baricentricFactor.v3)
                                                                        /invDenom).normalize();

                                    Vector3 pointToLight = getPointToLight(worldSpacePoint , *lightSource);
                                    Vector3 pointToLightDirection = pointToLight.normalize();
                                    double attenuation = lightSource->getAttenuation(pointToLight*(-1.0));

                                    shadowAmount = getShadowAmount(*lightSource, worldSpacePoint, faceNormal, fanTriangleRawWorldSpace);
                                    visibility = getVisibility(shadowAmount);

                                    bool brdf = obj.displaySettings->lightingMode == DisplaySettings::LightingModel::COOK_TORRANCE;


                                    combinedLight = getCombinedLight(Vectors::colorToVector3(lightSource->color), lightSource->intensity, attenuation);
                                    shadowTintModifier = getShadowTintModifier(kd, combinedLight, SHADOW_INTENSITY, shadowAmount);
                                    diffuseModifier = getDiffuseModifier(obj,combinedLight,worldSpacePoint,faceNormal,
                                                                         pointToLightDirection,kd,visibility, brdf);
                                    specularModifier = getSpecularModifier(obj, combinedLight, worldSpacePoint, faceNormal,
                                                                           pointToLightDirection, visibility);
                                    break;
                                }
                            }

                            outColor = outColor + diffuseModifier + specularModifier + shadowTintModifier;
                            lightId++;
                        }

                        outColor = MathUt::clampVector01(outColor);
                        finalColor = Vectors::vector3ToColor(RendUt::linearToSRGB(outColor));

                        if (paintTool.drawPixel(x, y, depth, finalColor)) {
                            IdBufferElement fillEl;
                            fillEl.objectId     = objId;
                            fillEl.faceId       = int(face/3);
                            (*idBuffer)[y][x] = fillEl;



                            if (!triDrew) {
                                stats.faces++;
                                stats.vertices += 3;
                                triDrew = true;
                                if(!objDrawn){
                                    stats.objects++;
                                }
                                objDrawn = true;
                            }

                        }
                    }
                }
            }
        }

        //pass wireframe - drawing edges of rasterizatied triangle
        if(obj.displaySettings->colorWireframes || obj.displaySettings->renderMode == DisplaySettings::RenderMode::WIREFRAME){
            size_t facePolysAmount = screenVerticesWithDepth.size();

            for (size_t i = 0; i < facePolysAmount; i++) {
                Vector3 edgeVertex1 = screenVerticesWithDepth[i];
                Vector3 edgeVertex2 = screenVerticesWithDepth[(i+1)%facePolysAmount];

                edgeVertex1.z = std::max(0.0, edgeVertex1.z - doubleBias);
                edgeVertex2.z = std::max(0.0, edgeVertex2.z - doubleBias);

                if(paintTool.drawLine3D(edgeVertex1, edgeVertex2,
                                         obj.displaySettings->colorWireframes? obj.viewportDisplay.wireframeColor : obj.viewportDisplay.color)){
                    stats.edges++;
                }
            }

        }

    }
}

double Renderer::getShadowAmount(const Light& light, const Vector3& worldSpacePoint, const Vector3& normal ,const Triangle3& worldSpaceFan){
    double shadowAmount = 0.0;

    size_t shadowMapfaceIndex = 0;
    if(light.lightType == Light::LightType::POINT){
        shadowMapfaceIndex = PointLight::pickFace(worldSpacePoint - light.transform.getPosition());
    }

    int kernelSide = FilteringManager::kernelSideFor(light);
    double bias = calculateBias(light, worldSpacePoint, normal, worldSpaceFan, kernelSide);

    Vector4 lightProjPoint = light.getProjectionMatrix() * light.getViewMatrix(shadowMapfaceIndex) * Vectors::vector3to4(worldSpacePoint);
    Vector3 lightNdcPoint = Vector3(lightProjPoint.x/lightProjPoint.w, lightProjPoint.y/lightProjPoint.w, lightProjPoint.z/lightProjPoint.w);


    float u =  lightNdcPoint.x * 0.5f + 0.5f;
    float v = 1.0f - (lightNdcPoint.y * 0.5f + 0.5f);
    if (MathUt::uvInTexture(u, v)){

        double receiverDepth;
        if (light.lightType == Light::LightType::DISTANT) {
            receiverDepth = lightNdcPoint.z*0.5 + 0.5;
        } else {
            const double dist = (worldSpacePoint - light.transform.getPosition()).length();
            receiverDepth = std::clamp((dist - light.near) / (light.range - light.near), 0.0, 1.0);
        }

        double sx = (lightNdcPoint.x * 0.5f + 0.5f) * (light.getShadowMap(shadowMapfaceIndex).getCols() - 1);
        double sy = (1 - (lightNdcPoint.y * 0.5f + 0.5f)) * (light.getShadowMap(shadowMapfaceIndex).getRows() - 1);

        shadowAmount  = calculateShadowAmount(light.getShadowMap(shadowMapfaceIndex), light,Vector2(sx,sy),
                                             receiverDepth, bias);
    }

    return shadowAmount;
}

Vector3 Renderer::getPointToLight(const Vector3& worldSpacePoint, const Light& light){
    if(light.lightType == Light::LightType::DISTANT){
        return light.direction*(-1.0);
    }else{
        return light.transform.getPosition() - worldSpacePoint;
    }
}

double Renderer::getVisibility(const double shadowAmount){
    return 1 - shadowAmount;
}

Vector3 Renderer::getShadowTintModifier(const Vector3& albedo, const Vector3& combinedLight,
                                        double shadowIntensity, double shadowAmount){
    return albedo.hadamard(combinedLight)*shadowIntensity*shadowAmount;
}

Vector3 Renderer::getCombinedLight(const Vector3& color, double intensity, double attenuation){
    return color*intensity*attenuation;
}

Vector3 Renderer::interpolateCombinedLight(const Triangle3& combinedLightOverW, const Triangle<double>& baricentricFactor, double invDenom){
    return (combinedLightOverW.v1 * baricentricFactor.v1 +
            combinedLightOverW.v2 * baricentricFactor.v2 +
            combinedLightOverW.v3 * baricentricFactor.v3)/invDenom;
}

Vector3 Renderer::interpolateDiffuseModifier(const Triangle3& diffuseNoAlbedoInVerticesOverW , const Triangle<double>& baricentricFactor,
                                             const Vector3& albedo, double invDenom, double visibility){

    Vector3 InterpolatedDiffuseNoAlbedo = (diffuseNoAlbedoInVerticesOverW.v1 * baricentricFactor.v1 +
                                   diffuseNoAlbedoInVerticesOverW.v2 * baricentricFactor.v2 +
                                   diffuseNoAlbedoInVerticesOverW.v3 * baricentricFactor.v3)/invDenom;

    Vector3 diffuseModifier = Vector3{InterpolatedDiffuseNoAlbedo.x * albedo.x,
                              InterpolatedDiffuseNoAlbedo.y * albedo.y,
                              InterpolatedDiffuseNoAlbedo.z * albedo.z} * visibility;

    return diffuseModifier;
}


Vector3 Renderer::interpolatSpecularModifier(const Triangle3& specularInVerticesOverW , const Triangle<double>& baricentricFactor,
                                     double invDenom, double visibility){
    Vector3 InterpolatedSpecular = (specularInVerticesOverW.v1 * baricentricFactor.v1 +
                                           specularInVerticesOverW.v2 * baricentricFactor.v2 +
                                           specularInVerticesOverW.v3 * baricentricFactor.v3)/invDenom;

    Vector3 specularModifier = InterpolatedSpecular * visibility;

    return specularModifier;
}


Vector3 Renderer::getDiffuseModifier(const RenderableObject3D& obj, const Vector3& combinedLight, const Vector3& pointWorldCoords,
                                     const Vector3& faceNormal, const Vector3& pointToLightDir, const Vector3& albedo, double visibility, bool brdf){

    Vector3 diffuseModifier = lightingManager->getReflectedLightLambert( pointToLightDir, faceNormal, combinedLight, albedo) * visibility;
    if(brdf){
        Vector3 brdfModifier = lightingManager->getDiffuseLambertBRDFMultiplier(pointToLightDir, obj.material, *camera, pointWorldCoords);
        return diffuseModifier.hadamard(brdfModifier);
    }else{
        return diffuseModifier;
    }

    diffuseModifier = diffuseModifier.hadamard(combinedLight) * visibility;
    return diffuseModifier;
}

Vector3 Renderer::getSpecularModifier(const RenderableObject3D& obj, const Vector3& combinedLight, const Vector3& pointWorldCoords,
                                   const Vector3& faceNormal, const Vector3& pointToLightDir, double visibility){

    Vector3 specularModifier;
    if(obj.displaySettings->lightingMode == DisplaySettings::LightingModel::PHONG){

        specularModifier = lightingManager->illuminatePointPhong(
            pointToLightDir, faceNormal, obj.material, *camera, pointWorldCoords
            );

    }else if(obj.displaySettings->lightingMode == DisplaySettings::LightingModel::BLINN_PHONG){

        specularModifier = lightingManager->illuminatePointBlinnPhong(
            pointToLightDir, faceNormal, obj.material, *camera, pointWorldCoords
            );

    }else if(obj.displaySettings->lightingMode == DisplaySettings::LightingModel::COOK_TORRANCE){

        specularModifier = lightingManager->getSpecularCookTorrance(
            pointToLightDir, faceNormal, obj.material, *camera, pointWorldCoords
            );

        specularModifier = specularModifier * (faceNormal.dotProduct(pointToLightDir));

    }

    specularModifier = specularModifier.hadamard(combinedLight) * visibility;
    return specularModifier;
}

Renderer::GouraudShadingFaceData Renderer::collectGouraudPerFaceData(
            Triangle<ClippingManager::ClippedVertex> fanTriangleClipped,
            const Triangle3& fanTriangleRawWorldSpace,
            const RenderableObject3D& obj){

    GouraudShadingFaceData result{};

    const Vector3 P1 = fanTriangleRawWorldSpace.v1;
    const Vector3 P2 = fanTriangleRawWorldSpace.v2;
    const Vector3 P3 = fanTriangleRawWorldSpace.v3;

    result.faceNormal = ((P2 - P1).crossProduct(P3 - P1)).normalize();
    result.centroid   = (P1 + P2 + P3) / 3.0;


    const double wInv1 = fanTriangleClipped.v1.invW;
    const double wInv2 = fanTriangleClipped.v2.invW;
    const double wInv3 = fanTriangleClipped.v3.invW;

    Vector3 N1 = (fanTriangleClipped.v1.worldSpaceNormalOverW * wInv1).normalize();
    Vector3 N2 = (fanTriangleClipped.v2.worldSpaceNormalOverW * wInv2).normalize();
    Vector3 N3 = (fanTriangleClipped.v3.worldSpaceNormalOverW * wInv3).normalize();

    auto initVertex = [&](GouraudShadingVertexData& vertexData) {
        const size_t lights = scene->lightSources.size();
        vertexData.combinedLightOverW.resize(lights, Vector3{});
        vertexData.diffuseNoAlbedoOverW.resize(lights, Vector3{});
        vertexData.specularOverW.resize(lights, Vector3{});
    };

    initVertex(result.face.v1);
    initVertex(result.face.v2);
    initVertex(result.face.v3);


    auto evalOneLightAtVertex = [&](const std::shared_ptr<Light>& Ls,
                                    const Vector3& P, const Vector3& N, double invW,
                                    Vector3& outCombinedOverW,
                                    Vector3& outDiffuseNoAlbOverW,
                                    Vector3& outSpecWithLightOverW)
    {
        Vector3 pointToLightDirection{};
        double  atten = 1.0;

        if (Ls->lightType == Light::LightType::DISTANT) {
            auto distantLight = std::static_pointer_cast<DistantLight>(Ls);
            pointToLightDirection  = (distantLight->direction * (-1.0)).normalize();
            atten = 1.0;

        } else if (Ls->lightType == Light::LightType::POINT) {
            auto pointLight = std::static_pointer_cast<PointLight>(Ls);
            Vector3 pointToLight = pointLight->transform.getPosition() - P;
            pointToLightDirection  = pointToLight.normalize();
            atten = pointLight->getAttenuation(pointToLight*(-1.0));

        } else { // SPOT
            auto spotLight = std::static_pointer_cast<SpotLight>(Ls);
            Vector3 pointToLight = spotLight->transform.getPosition() - P;
            Vector3 lightToPoint = pointToLight * (-1.0);
            pointToLightDirection  = pointToLight.normalize();
            atten = spotLight->getAttenuation(lightToPoint);
        }

        const Vector3 combined  = Vectors::colorToVector3(Ls->color) * Ls->intensity * atten;
        const double  ndotl     = std::max(0.0, N.dotProduct(pointToLightDirection));

        Vector3 diffNoAlb = combined * ndotl;

        Vector3 specNoLight{};
        if (obj.displaySettings->lightingMode == DisplaySettings::LightingModel::PHONG) {
                specNoLight = lightingManager->illuminatePointPhong(
                    const_cast<Vector3&>(pointToLightDirection), const_cast<Vector3&>(N),
                    obj.material, *camera, P
                    );
        } else if(obj.displaySettings->lightingMode == DisplaySettings::LightingModel::BLINN_PHONG) { // BLINN_PHONG
                specNoLight = lightingManager->illuminatePointBlinnPhong(
                    const_cast<Vector3&>(pointToLightDirection), const_cast<Vector3&>(N),
                    obj.material, *camera, P
                );

        }else if(obj.displaySettings->lightingMode == DisplaySettings::LightingModel::COOK_TORRANCE) { // BLINN_PHONG
            specNoLight = lightingManager->getSpecularCookTorrance(
                const_cast<Vector3&>(pointToLightDirection), const_cast<Vector3&>(N),
                obj.material, *camera, P
                );
            specNoLight = specNoLight * ndotl;

            diffNoAlb = diffNoAlb.hadamard(lightingManager->getDiffuseLambertBRDFMultiplier(pointToLightDirection, obj.material, *camera, P));

        }else{
            specNoLight = {};
        }
        const Vector3 specWithLight = { specNoLight.x * combined.x,
                                       specNoLight.y * combined.y,
                                       specNoLight.z * combined.z };

        outCombinedOverW      = combined      * invW;
        outDiffuseNoAlbOverW  = diffNoAlb     * invW;
        outSpecWithLightOverW = specWithLight * invW;
    };

    for (size_t i = 0; i < scene->lightSources.size(); i++) {
        auto& lightSource = scene->lightSources[i];

        evalOneLightAtVertex(lightSource, P1, N1, wInv1,
                             result.face.v1.combinedLightOverW[i],
                             result.face.v1.diffuseNoAlbedoOverW[i],
                             result.face.v1.specularOverW[i]);

        evalOneLightAtVertex(lightSource, P2, N2, wInv2,
                             result.face.v2.combinedLightOverW[i],
                             result.face.v2.diffuseNoAlbedoOverW[i],
                             result.face.v2.specularOverW[i]);

        evalOneLightAtVertex(lightSource, P3, N3, wInv3,
                             result.face.v3.combinedLightOverW[i],
                             result.face.v3.diffuseNoAlbedoOverW[i],
                             result.face.v3.specularOverW[i]);
    }

    return result;
}

Renderer::FlatShadingFaceData Renderer::collectFlatPerFaceData(const Triangle3& fanTriangleRawWorldSpace,
                                           const RenderableObject3D& obj){

    FlatShadingFaceData result{};
    result.pointToLightDirection.assign(scene->lightSources.size() , {});
    result.combinedLight.assign(scene->lightSources.size() , {});
    result.diffuseNoAlbedo.assign(scene->lightSources.size() , {});
    result.specularWithLight.assign(scene->lightSources.size() , {});
    result.NdotL.assign(scene->lightSources.size() , {});

    const Vector3 w0 = fanTriangleRawWorldSpace.v1;
    const Vector3 w1 = fanTriangleRawWorldSpace.v2;
    const Vector3 w2 = fanTriangleRawWorldSpace.v3;

    result.flatFaceNormal = ((w1 - w0).crossProduct(w2 - w0)).normalize();
    result.flatWorldCentroid = (w0 + w1 + w2) / 3.0;

    for (size_t i = 0; i < scene->lightSources.size(); ++i) {
        auto& lightSource = scene->lightSources[i];

        Vector3 pointToLightDirection{};
        double  attenuation = 1.0;

        if (lightSource->lightType == Light::LightType::DISTANT) {
            auto distantLight = std::static_pointer_cast<DistantLight>(lightSource);
            pointToLightDirection = (distantLight->direction * (-1.0)).normalize();
            attenuation = 1.0;

        } else if (lightSource->lightType == Light::LightType::POINT) {
            auto pointLight = std::static_pointer_cast<PointLight>(lightSource);
            Vector3 pointToLightVector = (pointLight->transform.getPosition() - result.flatWorldCentroid);
            pointToLightDirection = pointToLightVector.normalize();
            attenuation = pointLight->getAttenuation(pointToLightVector);

        } else if (lightSource->lightType == Light::LightType::SPOT) {
            auto spotLight = std::static_pointer_cast<SpotLight>(lightSource);

            Vector3 pointToLightVector = (spotLight->transform.getPosition() - result.flatWorldCentroid);
            Vector3 lightToPointVector = pointToLightVector * (-1.0);
            pointToLightDirection = pointToLightVector.normalize();
            attenuation = spotLight->getAttenuation(lightToPointVector);

        }

        Vector3 combinedLight = Vectors::colorToVector3(lightSource->color) * lightSource->intensity * attenuation;
        double nDotL = std::max(0.0, result.flatFaceNormal.dotProduct(pointToLightDirection));

        result.pointToLightDirection[i] = pointToLightDirection;
        result.combinedLight[i]         = combinedLight;
        result.NdotL[i] = nDotL;

        result.specularWithLight[i] = getSpecularModifier(obj,combinedLight, result.flatWorldCentroid, result.flatFaceNormal, pointToLightDirection);

        bool brdf = obj.displaySettings->lightingMode == DisplaySettings::LightingModel::COOK_TORRANCE;
        result.diffuseNoAlbedo[i] = getDiffuseModifier(obj, combinedLight, result.flatWorldCentroid,result.flatFaceNormal,
                                                       pointToLightDirection, Vector3{1,1,1}, 1.0, brdf );
    }

    return result;
}
void Renderer::updateShadowMaps(){

    if(debugMode == DEBUG_SHADOWMAP){
        std::cout << "started updateShadowMaps, number of light sources on the scene: "<<scene->lightSources.size()<<std::endl;
    }

    std::vector<Vector3> cameraFrustum = camera->getFrustum();

    //filling shadow maps of light sources
    for(std::shared_ptr<Light>&lightSource : scene->lightSources){
        if(debugMode == DEBUG_SHADOWMAP){
            //   std::cout << "if start" <<std::endl;
        }
        //cast Light to its proper subclass
        if(lightSource->lightType == Light::LightType::DISTANT){
            if(debugMode == DEBUG_SHADOWMAP){
                std::cout << "Found distant light source"<<std::endl;
            }
            auto distantLight = std::static_pointer_cast<DistantLight>(lightSource);
            double INF = std::numeric_limits<double>::infinity();;


                //get lightview Matrix
                Vector3 bboxCenter = distantLight->getBBoxCenter(cameraFrustum);
                distantLight->setViewMatrix(bboxCenter);
                Matrix4 lightView = distantLight->getViewMatrix();

                if(debugMode == DEBUG_SHADOWMAP){
                    std::cout<<"Eye: "<<bboxCenter<<std::endl;
                    std::cout<<"light View: \n"<<lightView<<"\n"<<std::endl;
                }

                std::vector<Vector3> lightSpaceCameraFrustum;
                for(Vector3& v : cameraFrustum){
                    lightSpaceCameraFrustum.push_back(Vectors::vector4to3(lightView * Vectors::vector3to4(v)));
                }

                //calculating bounding box in light space
                double minX = lightSpaceCameraFrustum[0].x, maxX = lightSpaceCameraFrustum[0].x,
                    minY = lightSpaceCameraFrustum[0].y, maxY = lightSpaceCameraFrustum[0].y,
                    minZ = lightSpaceCameraFrustum[0].z, maxZ = lightSpaceCameraFrustum[0].z;



                for(Vector3& v : lightSpaceCameraFrustum){
                    if(v.x > maxX) maxX = v.x;
                    else if(v.x < minX ) minX = v.x;
                    if(v.y > maxY) maxY = v.y;
                    else if(v.y < minY ) minY = v.y;
                    if(v.z > maxZ) maxZ = v.z;
                    else if(v.z < minZ ) minZ = v.z;
                }

                const double padXY = 0.05 * std::max(maxX - minX, maxY - minY);
                const double padZ  = 0.05 * (maxZ - minZ);

                // light backoff
                double shift = maxZ + padZ;                     // point closest to camera
                Matrix4 backOff = Matrices4::translation(0,0,-shift);
                distantLight->setViewMatrix(backOff * lightView);

                // calculate bbox Z after backOff
                for (auto& v : cameraFrustum) {
                    Vector3 inLS = Vectors::vector4to3(distantLight->getViewMatrix() * Vectors::vector3to4(v));
                    minZ = std::min(minZ, inLS.z);
                    maxZ = std::max(maxZ, inLS.z);
                }

                // calculate near and far and set proj matrix
                double nearDist = lightSource->near;
                double farDist  = (-minZ) + padZ;
                //double farDist  = 500;
                //double nearDist = l


                distantLight->setProjectionMatrix(
                    minX - padXY, maxX + padXY,
                    minY - padXY, maxY + padXY,
                    nearDist, farDist
                    );


                Matrix4 lightProjection = distantLight->getProjectionMatrix();
                if(debugMode == DEBUG_SHADOWMAP){
                    std::cout << "Ortho projection:\n" << lightProjection << std::endl;

                    std::cout<<"bbox in lightviewproj: "<<std::endl;
                    for(Vector3& v : cameraFrustum){
                        Vector4 v4 = Vectors::vector3to4(v);
                        Vector4 proj = lightProjection * distantLight->getViewMatrix() * v4;
                        Vector3 ndc = Vector3(proj.x / proj.w, proj.y / proj.w, proj.z / proj.w);
                        std::cout << "NDC: " << ndc << std::endl;
                    }
                }
                shadowMapDepthPass((*distantLight));
                //depth pass - filling shadow map of current light source


        }else if(lightSource->lightType == Light::LightType::POINT){
            if(debugMode == DEBUG_SHADOWMAP){
                std::cout << "Found point light source"<<std::endl;
            }
            std::shared_ptr<PointLight> pointLight = std::static_pointer_cast<PointLight>(lightSource);


                //get light view matrices
                pointLight->setViewMatrices();

                double near = lightSource->near;
                double far = pointLight->range;

                pointLight->setProjectionMatrix(near, far);
                if(debugMode == DEBUG_SHADOWMAP){
                    std::cout << "perspective projection:\n" << pointLight->getProjectionMatrix() << std::endl;
                }

                shadowMapDepthPass((*pointLight));




        }else if(lightSource->lightType == Light::LightType::SPOT){
            if(debugMode == DEBUG_SHADOWMAP){
                std::cout << "somehow found spot light source"<<std::endl;
            }
            auto spotLight = std::static_pointer_cast<SpotLight>(lightSource);

                spotLight->setViewMatrix();

                double near = lightSource->near;
                double far = spotLight->range;

                spotLight->setProjectionMatrix(near, far);
                if(debugMode == DEBUG_SHADOWMAP){
                    std::cout << "perspective projection:\n" << spotLight->getProjectionMatrix() << std::endl;
                }

                shadowMapDepthPass((*spotLight));

        }else{
            if(debugMode == DEBUG_SHADOWMAP){
                std::cout << "found inidentified light source"<<std::endl;
            }
        }
        if(debugMode == DEBUG_SHADOWMAP){
            // std::cout << "if end" <<std::endl;
        }
    }
}

void Renderer::shadowMapDepthPass(DistantLight& lightSource){
    //clear shadowMap
    lightSource.shadowMap.clear();

    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){
        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){
            if(curObject->displaySettings->renderMode == DisplaySettings::RenderMode::NONE) continue;

            Matrix4 MVP = lightSource.getProjectionMatrix() * lightSource.getViewMatrix() * curObject->transform.getTransMatrix();

            //clipping preprocess
            std::vector<ClippingManager::ClippedVertex> clipVertices;
            for(size_t i = 0; i < curObject->vertices.size(); i++){
                const Vector3& localSpaceVertex = curObject->vertices[i];


                Vector4 clipSpaceVertex = MVP * Vectors::vector3to4(localSpaceVertex);
                Vector3 worldSpaceVertex = modelToWorld(localSpaceVertex, curObject->transform.getTransMatrix());
                double invertedW = 1.0/clipSpaceVertex.w;
                clipVertices.push_back(
                    {
                        clipSpaceVertex,
                        invertedW,
                        worldSpaceVertex * invertedW,
                        Vector3(),
                        Vector3()
                    }
                    );
            }

            for (size_t face = 0; face < curObject->faceVertexIndices.size(); face += 3)
            {

                //original vertices in clip-space
                ClippingManager::ClippedVertex cv0 = clipVertices[curObject->faceVertexIndices[face    ]];
                ClippingManager::ClippedVertex cv1 = clipVertices[curObject->faceVertexIndices[face + 1]];
                ClippingManager::ClippedVertex cv2 = clipVertices[curObject->faceVertexIndices[face + 2]];

                //clipping triangle
                std::vector<ClippingManager::ClippedVertex> clippedPoly = clippingManager->clipTriangle({cv0, cv1, cv2});
                if (clippedPoly.size() < 3) continue;

                // clip -> ndc -> screen - > screen with normalized depth
                std::vector<Vector3> screenVerticesWithDepth;
                screenVerticesWithDepth.reserve(clippedPoly.size());
                for (ClippingManager::ClippedVertex& cv : clippedPoly){
                    Vector3 normalizedClipVertex = clipToNdc(cv.clip);
                    if(debugMode == DEBUG_SHADOWMAP){
                        if (std::abs(normalizedClipVertex.x) > 1.001 ||
                            std::abs(normalizedClipVertex.y) > 1.001 ||
                            std::abs(normalizedClipVertex.z) > 1.001) {
                            std::cerr << "[WARN] Out of NDC: "
                                      << normalizedClipVertex.x << ", "
                                      << normalizedClipVertex.y << ", "
                                      << normalizedClipVertex.z << " (before clamp)\n";
                        }
                    }
                    normalizedClipVertex.x = std::clamp(normalizedClipVertex.x, -1.0, 1.0);
                    normalizedClipVertex.y = std::clamp(normalizedClipVertex.y, -1.0, 1.0);

                    //Vector2 screenSpaceVertex = Vector2(int(std::round( (normalizedClipVertex.x * 0.5 + 0.5) * (lightSource.shadowMap.getCols()-1) ) ) ,
                     //                                   std::round(int( (1 - (normalizedClipVertex.y*0.5+0.5)) * (lightSource.shadowMap.getRows()-1) )));
                    const double sx = (normalizedClipVertex.x*0.5 + 0.5) * (lightSource.shadowMap.getCols()-1);
                    const double sy = (1.0 - (normalizedClipVertex.y*0.5 + 0.5)) * (lightSource.shadowMap.getRows()-1);
                    Vector2 screenSpaceVertex = Vector2(sx, sy);


                    screenVerticesWithDepth.push_back(Vector3(screenSpaceVertex.x , screenSpaceVertex.y, ((cv.clip.z/cv.clip.w)+1.0)*0.5));

                    if(debugMode == DEBUG_SHADOWMAP){
                        if (screenSpaceVertex.x < 0 || screenSpaceVertex.x >= lightSource.shadowMap.getCols() ||
                            screenSpaceVertex.y < 0 || screenSpaceVertex.y >= lightSource.shadowMap.getRows()) {
                            std::cerr << "[WARN] Out of SM: ("
                                      << screenSpaceVertex.x << ", "
                                      << screenSpaceVertex.y << ") of "
                                      << lightSource.shadowMap.getCols() << "x"
                                      << lightSource.shadowMap.getRows() << "\n";

                        }
                        float depth = screenVerticesWithDepth.front().z;
                        if (depth < -0.001 || depth > 1.001) {
                            std::cerr << "[WARN] Bad depth: " << depth << "\n";
                        }
                        //std::cout<<"vertex in Screen: "<<screenVerticesWithDepth.front()<<std::endl;
                    }
                }



                //Fill-pass
                for (size_t k = 1; k + 1 < clippedPoly.size(); k++)
                {
                    // Fan-triangulation
                    Vector3& A = screenVerticesWithDepth[0];
                    Vector3& B = screenVerticesWithDepth[k];
                    Vector3& C = screenVerticesWithDepth[k+1];

                    ClippingManager::ClippedVertex v0 = clippedPoly[0];
                    ClippingManager::ClippedVertex v1 = clippedPoly[k];
                    ClippingManager::ClippedVertex v2 = clippedPoly[k+1];

                    // bounding box for rasterization
                    // beyond this rectangle there's no need to make further checks
                    int minX = std::max(0, int(std::floor(std::min({A.x,B.x,C.x}))));
                    int maxX = std::min((int)lightSource.shadowMap.getCols()-1, int(std::ceil(std::max({A.x,B.x,C.x}))));
                    int minY = std::max(0, int(std::floor(std::min({A.y,B.y,C.y}))));
                    int maxY = std::min((int)lightSource.shadowMap.getRows()-1, int(std::ceil(std::max({A.y,B.y,C.y}))));

                    //calculating area of triangle
                    //if triangle's area = 0, nothing to raster
                    //inv area will be used in further calculations
                    double area = (B.x - A.x)*(C.y - A.y)
                                  - (B.y - A.y)*(C.x - A.x);
                    if (std::abs(area) < 1e-8) continue;
                    double invArea = 1.0 / area;


                    for (int y = minY; y <= maxY; y++) {
                        for (int x = minX; x <= maxX; x++) {

                            double px = x + 0.5, py = y + 0.5; //middle of current pixel

                            //checking if pixel is withing triangle using baricentric coordinates
                            double w0 = ((B.x-px)*(C.y-py) - (B.y-py)*(C.x-px)) * invArea;
                            double w1 = ((C.x-px)*(A.y-py) - (C.y-py)*(A.x-px)) * invArea;
                            double w2 = 1.0 - w0 - w1;
                            if (w0<0 || w1<0 || w2<0) continue;

                            double invDenom = w0 * v0.invW + w1 * v1.invW + w2 * v2.invW;
                            Vector3 interpolatedWorldSpaceCoords = (v0.worldSpaceVertexOverW*w0 + v1.worldSpaceVertexOverW*w1 + v2.worldSpaceVertexOverW*w2)/invDenom;

                            Vector4 lightProjCoord = lightSource.getProjectionMatrix() * lightSource.getViewMatrix() * Vectors::vector3to4(interpolatedWorldSpaceCoords);
                            Vector3 lightNdcCoord = Vector3(lightProjCoord.x/lightProjCoord.w, lightProjCoord.y/lightProjCoord.w, lightProjCoord.z/lightProjCoord.w);
                            float depthInLightView = lightNdcCoord.z * 0.5f + 0.5f;

                            if (depthInLightView < lightSource.shadowMap[y][x])
                                lightSource.shadowMap[y][x] = depthInLightView;
                        }
                    }

                }
            }

        }
    }


    //lightSource.printShadowMatrix();
    if(debugMode == DEBUG_SHADOWMAP){
        lightSource.printShadowMatrix();
        std::cout<<"ShadowMap non-empty elements: "<<lightSource.shadowMap.nonEmptyElements()<<"//"<<lightSource.shadowMap.size()<<std::endl;
        lightSource.shadowMap.logNonEmptyElements();
    }
}

void Renderer::shadowMapDepthPass(PointLight& lightSource){

    //clear shadowMaps
    for(auto map: lightSource.shadowMaps){
        map->clear();
    }

    //std::vector<Matrix4> MVP(6 , Matrices4::identity());
    if(debugMode == DEBUG_SHADOWMAP){
        // std::cout<<"starting shadowMap depth pass"<<std::endl;
    }
    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){
        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){
            if(curObject->displaySettings->renderMode == DisplaySettings::RenderMode::NONE) continue;
            //std::cout<<"1"<<std::endl;
            for(int shadowCubeFace = 0; shadowCubeFace < 6; shadowCubeFace++){
                Matrix4 MVP = lightSource.getProjectionMatrix() * lightSource.getViewMatrix(shadowCubeFace) * curObject->transform.getTransMatrix();

                //std::cout<<"2"<<std::endl;
                //clipping preprocess
                std::vector<ClippingManager::ClippedVertex> clipVertices;
                for(size_t i = 0; i < curObject->vertices.size(); i++){
                    const Vector3& localSpaceVertex = curObject->vertices[i];


                    Vector4 clipSpaceVertex = MVP * Vectors::vector3to4(localSpaceVertex);
                    Vector3 worldSpaceVertex = modelToWorld(localSpaceVertex, curObject->transform.getTransMatrix());
                    double invertedW = 1.0/clipSpaceVertex.w;
                    clipVertices.push_back(
                        {
                            clipSpaceVertex,
                            invertedW,
                            worldSpaceVertex * invertedW,
                            Vector3(),
                            Vector3()
                        }
                        );
                }
                for (size_t face = 0; face < curObject->faceVertexIndices.size(); face += 3)
                {
                    //original vertices in clip-space
                    ClippingManager::ClippedVertex cv0 = clipVertices[curObject->faceVertexIndices[face    ]];
                    ClippingManager::ClippedVertex cv1 = clipVertices[curObject->faceVertexIndices[face + 1]];
                    ClippingManager::ClippedVertex cv2 = clipVertices[curObject->faceVertexIndices[face + 2]];

                    //clipping triangle
                    std::vector<ClippingManager::ClippedVertex> clippedPoly = clippingManager->clipTriangle({cv0, cv1, cv2});
                    if (clippedPoly.size() < 3) continue;

                    // clip -> ndc -> screen - > screen with normalized depth
                    std::vector<Vector3> screenVerticesWithDepth;
                    screenVerticesWithDepth.reserve(clippedPoly.size());
                    for (ClippingManager::ClippedVertex& cv : clippedPoly){
                        Vector3 normalizedClipVertex = clipToNdc(cv.clip);
                        if(debugMode == DEBUG_SHADOWMAP){
                            if (std::abs(normalizedClipVertex.x) > 1.001 ||
                                std::abs(normalizedClipVertex.y) > 1.001 ||
                                std::abs(normalizedClipVertex.z) > 1.001) {
                                std::cerr << "[WARN] Out of NDC: "
                                          << normalizedClipVertex.x << ", "
                                          << normalizedClipVertex.y << ", "
                                          << normalizedClipVertex.z << " (before clamp)\n";
                            }
                        }
                        normalizedClipVertex.x = std::clamp(normalizedClipVertex.x, -1.0, 1.0);
                        normalizedClipVertex.y = std::clamp(normalizedClipVertex.y, -1.0, 1.0);


                        //Vector2 screenSpaceVertex = Vector2(int(std::round( (normalizedClipVertex.x * 0.5 + 0.5) * (lightSource.shadowMaps[shadowCubeFace]->getCols()-1) ) ) ,
                          //                                  std::round(int( (1 - (normalizedClipVertex.y*0.5+0.5)) * (lightSource.shadowMaps[shadowCubeFace]->getRows()-1) )));

                        Vector2 screenSpaceVertex = Vector2((( (normalizedClipVertex.x * 0.5 + 0.5) * (lightSource.shadowMaps[shadowCubeFace]->getCols()-1) ) ) ,
                                                            (( (1 - (normalizedClipVertex.y*0.5+0.5)) * (lightSource.shadowMaps[shadowCubeFace]->getRows()-1) )));

                        screenVerticesWithDepth.push_back(Vector3(screenSpaceVertex.x , screenSpaceVertex.y, ((cv.clip.z/cv.clip.w)+1.0)*0.5));

                        if(debugMode == DEBUG_SHADOWMAP){
                            if (screenSpaceVertex.x < 0 || screenSpaceVertex.x >= lightSource.shadowMaps[shadowCubeFace]->getCols() ||
                                screenSpaceVertex.y < 0 || screenSpaceVertex.y >= lightSource.shadowMaps[shadowCubeFace]->getRows()) {
                                std::cerr << "[WARN] Out of SM: ("
                                          << screenSpaceVertex.x << ", "
                                          << screenSpaceVertex.y << ") of "
                                          << lightSource.shadowMaps[shadowCubeFace]->getCols() << "x"
                                          << lightSource.shadowMaps[shadowCubeFace]->getRows() << "\n";

                            }
                            float depth = screenVerticesWithDepth.front().z;
                            if (depth < -0.001 || depth > 1.001) {
                                std::cerr << "[WARN] Bad depth: " << depth << "\n";
                            }
                            //std::cout<<"vertex in Screen: "<<screenVerticesWithDepth.front()<<std::endl;
                        }
                    }



                    //Fill-pass
                    for (size_t k = 1; k + 1 < clippedPoly.size(); k++)
                    {
                        // Fan-triangulation
                        Vector3& A = screenVerticesWithDepth[0];
                        Vector3& B = screenVerticesWithDepth[k];
                        Vector3& C = screenVerticesWithDepth[k+1];

                        ClippingManager::ClippedVertex v0 = clippedPoly[0];
                        ClippingManager::ClippedVertex v1 = clippedPoly[k];
                        ClippingManager::ClippedVertex v2 = clippedPoly[k+1];

                        // bounding box for rasterization
                        // beyond this rectangle there's no need to make further checks
                        int minX = std::max(0, int(std::floor(std::min({A.x,B.x,C.x}))));
                        int maxX = std::min((int)lightSource.shadowMaps[shadowCubeFace]->getCols()-1, int(std::ceil(std::max({A.x,B.x,C.x}))));
                        int minY = std::max(0, int(std::floor(std::min({A.y,B.y,C.y}))));
                        int maxY = std::min((int)lightSource.shadowMaps[shadowCubeFace]->getRows()-1, int(std::ceil(std::max({A.y,B.y,C.y}))));

                        //calculating area of triangle
                        //if triangle's area = 0, nothing to raster
                        //inv area will be used in further calculations
                        double area = (B.x - A.x)*(C.y - A.y)
                                      - (B.y - A.y)*(C.x - A.x);
                        if (std::abs(area) < 1e-8) continue;
                        double invArea = 1.0 / area;


                        for (int y = minY; y <= maxY; y++) {
                            for (int x = minX; x <= maxX; x++) {

                                double px = x + 0.5, py = y + 0.5; //middle of current pixel

                                //checking if pixel is withing triangle using baricentric coordinates
                                double w0 = ((B.x-px)*(C.y-py) - (B.y-py)*(C.x-px)) * invArea;
                                double w1 = ((C.x-px)*(A.y-py) - (C.y-py)*(A.x-px)) * invArea;
                                double w2 = 1.0 - w0 - w1;
                                if (w0<0 || w1<0 || w2<0) continue;

                                double invDenom = w0 * v0.invW + w1 * v1.invW + w2 * v2.invW;
                                Vector3 interpolatedWorldSpaceCoords = (v0.worldSpaceVertexOverW*w0 + v1.worldSpaceVertexOverW*w1 + v2.worldSpaceVertexOverW*w2)/invDenom;

                                //Vector4 lightProjCoord = lightSource.getProjectionMatrix() * lightSource.getViewMatrix(shadowCubeFace) * Vectors::vector3to4(interpolatedWorldSpaceCoords);
                                //Vector3 lightNdcCoord = Vector3(lightProjCoord.x/lightProjCoord.w, lightProjCoord.y/lightProjCoord.w, lightProjCoord.z/lightProjCoord.w);
                                //float depthInLightView = lightNdcCoord.z * 0.5f + 0.5f;
                                float depthInLightView = (interpolatedWorldSpaceCoords - lightSource.transform.getPosition()).length();
                                float normalizedDepth = std::clamp((depthInLightView - lightSource.near) / (lightSource.range - lightSource.near), 0.0, 1.0);

                                if (normalizedDepth < (*lightSource.shadowMaps[shadowCubeFace])[y][x]){
                                    (*lightSource.shadowMaps[shadowCubeFace])[y][x] = normalizedDepth;
                                    //(*lightSource.shadowMaps[shadowCubeFace])[y][x] = std::min(lightSource.range, (*lightSource.shadowMaps[shadowCubeFace])[y][x]);
                                }
                            }
                        }

                    }
                }
            }

        }
    }
    // lightSource.printShadowMaps();
    if(debugMode == DEBUG_SHADOWMAP){

        lightSource.printShadowMaps();
        for(int face = 0; face<6; face++){
            std::cout<<"ShadowMap "<<face<<" non-empty elements: "<<lightSource.shadowMaps[face]->nonEmptyElements()<<"//"<<lightSource.shadowMaps[face]->size()<<std::endl;
            std::cout<<"lightView matrix "<<face<<" :\n"<<lightSource.getViewMatrix(face)<<std::endl;
        }
        //lightSource.shadowMap.logNonEmptyElements();

    }
}

void Renderer::shadowMapDepthPass(SpotLight& lightSource){

    lightSource.shadowMap.clear();

    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){
        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){
            if(curObject->displaySettings->renderMode == DisplaySettings::RenderMode::NONE) continue;
            //std::cout<<"1"<<std::endl;

                Matrix4 MVP = lightSource.getProjectionMatrix() * lightSource.getViewMatrix() * curObject->transform.getTransMatrix();

                //std::cout<<"2"<<std::endl;
                //clipping preprocess
                std::vector<ClippingManager::ClippedVertex> clipVertices;
                for(size_t i = 0; i < curObject->vertices.size(); i++){
                    const Vector3& localSpaceVertex = curObject->vertices[i];


                    Vector4 clipSpaceVertex = MVP * Vectors::vector3to4(localSpaceVertex);
                    Vector3 worldSpaceVertex = modelToWorld(localSpaceVertex, curObject->transform.getTransMatrix());
                    double invertedW = 1.0/clipSpaceVertex.w;
                    clipVertices.push_back(
                        {
                            clipSpaceVertex,
                            invertedW,
                            worldSpaceVertex * invertedW,
                            Vector3(),
                            Vector3()
                        }
                        );
                }
                for (size_t face = 0; face < curObject->faceVertexIndices.size(); face += 3)
                {
                    //original vertices in clip-space
                    ClippingManager::ClippedVertex cv0 = clipVertices[curObject->faceVertexIndices[face    ]];
                    ClippingManager::ClippedVertex cv1 = clipVertices[curObject->faceVertexIndices[face + 1]];
                    ClippingManager::ClippedVertex cv2 = clipVertices[curObject->faceVertexIndices[face + 2]];

                    //clipping triangle
                    std::vector<ClippingManager::ClippedVertex> clippedPoly = clippingManager->clipTriangle({cv0, cv1, cv2});
                    if (clippedPoly.size() < 3) continue;

                    // clip -> ndc -> screen - > screen with normalized depth
                    std::vector<Vector3> screenVerticesWithDepth;
                    screenVerticesWithDepth.reserve(clippedPoly.size());
                    for (ClippingManager::ClippedVertex& cv : clippedPoly){
                        Vector3 normalizedClipVertex = clipToNdc(cv.clip);
                        if(debugMode == DEBUG_SHADOWMAP){
                            if (std::abs(normalizedClipVertex.x) > 1.001 ||
                                std::abs(normalizedClipVertex.y) > 1.001 ||
                                std::abs(normalizedClipVertex.z) > 1.001) {
                                std::cerr << "[WARN] Out of NDC: "
                                          << normalizedClipVertex.x << ", "
                                          << normalizedClipVertex.y << ", "
                                          << normalizedClipVertex.z << " (before clamp)\n";
                            }
                        }
                        normalizedClipVertex.x = std::clamp(normalizedClipVertex.x, -1.0, 1.0);
                        normalizedClipVertex.y = std::clamp(normalizedClipVertex.y, -1.0, 1.0);


                        //Vector2 screenSpaceVertex = Vector2(int(std::round( (normalizedClipVertex.x * 0.5 + 0.5) * (lightSource.shadowMaps[shadowCubeFace]->getCols()-1) ) ) ,
                        //                                  std::round(int( (1 - (normalizedClipVertex.y*0.5+0.5)) * (lightSource.shadowMaps[shadowCubeFace]->getRows()-1) )));

                        Vector2 screenSpaceVertex = Vector2((( (normalizedClipVertex.x * 0.5 + 0.5) * (lightSource.shadowMap.getCols()-1) ) ) ,
                                                            (( (1 - (normalizedClipVertex.y*0.5+0.5)) * (lightSource.shadowMap.getRows()-1) )));

                        screenVerticesWithDepth.push_back(Vector3(screenSpaceVertex.x , screenSpaceVertex.y, ((cv.clip.z/cv.clip.w)+1.0)*0.5));

                        if(debugMode == DEBUG_SHADOWMAP){
                            if (screenSpaceVertex.x < 0 || screenSpaceVertex.x >= lightSource.shadowMap.getCols() ||
                                screenSpaceVertex.y < 0 || screenSpaceVertex.y >= lightSource.shadowMap.getRows()) {
                                std::cerr << "[WARN] Out of SM: ("
                                          << screenSpaceVertex.x << ", "
                                          << screenSpaceVertex.y << ") of "
                                          << lightSource.shadowMap.getCols() << "x"
                                          << lightSource.shadowMap.getRows() << "\n";

                            }
                            float depth = screenVerticesWithDepth.front().z;
                            if (depth < -0.001 || depth > 1.001) {
                                std::cerr << "[WARN] Bad depth: " << depth << "\n";
                            }
                            //std::cout<<"vertex in Screen: "<<screenVerticesWithDepth.front()<<std::endl;
                        }
                    }



                    //Fill-pass
                    for (size_t k = 1; k + 1 < clippedPoly.size(); k++)
                    {
                        // Fan-triangulation
                        Vector3& A = screenVerticesWithDepth[0];
                        Vector3& B = screenVerticesWithDepth[k];
                        Vector3& C = screenVerticesWithDepth[k+1];

                        ClippingManager::ClippedVertex v0 = clippedPoly[0];
                        ClippingManager::ClippedVertex v1 = clippedPoly[k];
                        ClippingManager::ClippedVertex v2 = clippedPoly[k+1];

                        // bounding box for rasterization
                        // beyond this rectangle there's no need to make further checks
                        int minX = std::max(0, int(std::floor(std::min({A.x,B.x,C.x}))));
                        int maxX = std::min((int)lightSource.shadowMap.getCols()-1, int(std::ceil(std::max({A.x,B.x,C.x}))));
                        int minY = std::max(0, int(std::floor(std::min({A.y,B.y,C.y}))));
                        int maxY = std::min((int)lightSource.shadowMap.getRows()-1, int(std::ceil(std::max({A.y,B.y,C.y}))));

                        //calculating area of triangle
                        //if triangle's area = 0, nothing to raster
                        //inv area will be used in further calculations
                        double area = (B.x - A.x)*(C.y - A.y)
                                      - (B.y - A.y)*(C.x - A.x);
                        if (std::abs(area) < 1e-8) continue;
                        double invArea = 1.0 / area;


                        for (int y = minY; y <= maxY; y++) {
                            for (int x = minX; x <= maxX; x++) {

                                double px = x + 0.5, py = y + 0.5; //middle of current pixel

                                //checking if pixel is withing triangle using baricentric coordinates
                                double w0 = ((B.x-px)*(C.y-py) - (B.y-py)*(C.x-px)) * invArea;
                                double w1 = ((C.x-px)*(A.y-py) - (C.y-py)*(A.x-px)) * invArea;
                                double w2 = 1.0 - w0 - w1;
                                if (w0<0 || w1<0 || w2<0) continue;

                                double invDenom = w0 * v0.invW + w1 * v1.invW + w2 * v2.invW;
                                Vector3 interpolatedWorldSpaceCoords = (v0.worldSpaceVertexOverW*w0 + v1.worldSpaceVertexOverW*w1 + v2.worldSpaceVertexOverW*w2)/invDenom;

                               // Vector4 lightProjCoord = lightSource.getProjectionMatrix() * lightSource.getViewMatrix() * Vectors::vector3to4(interpolatedWorldSpaceCoords);
                               // Vector3 lightNdcCoord = Vector3(lightProjCoord.x/lightProjCoord.w, lightProjCoord.y/lightProjCoord.w, lightProjCoord.z/lightProjCoord.w);
                               // float depthInLightView = lightNdcCoord.z * 0.5f + 0.5f;

                                float depthInLightView = (interpolatedWorldSpaceCoords - lightSource.transform.getPosition()).length();
                                float normalizedDepth = std::clamp((depthInLightView-lightSource.near)/(lightSource.range - lightSource.near),0.0,1.0);

                                if (normalizedDepth < lightSource.shadowMap[y][x]){
                                    lightSource.shadowMap[y][x] = normalizedDepth;
                                    //lightSource.shadowMap[y][x] = std::min(lightSource.range, lightSource.shadowMap[y][x]);
                                }
                            }
                        }

                }
            }
        }
        //lightSource.printShadowMatrix();
    }
}

bool Renderer::shouldCullBackFace(const Triangle3& face){
    Vector3 cameraPointVector = camera->transform.getPosition() - face.v1;
    return getFaceNormal(face.v1, face.v2, face.v3).dotProduct(cameraPointVector) <= 0;
}

/*
void Renderer::calculateBias(const std::shared_ptr<Light>& light, const Vector3& point,
                                const Vector3& normal, Vector3& pointForDepthCheck, double& biasAddition,
                                Triangle3& fanWorldCoords,  PointLight::ShadowMapFace* outFace, int pcfKernelSize){
    using BT = Light::BiasType;

    switch (light->biasType) {
    case BT::NORMAL_ANGLE: {
        if (light->lightType == Light::LightType::DISTANT) {

            auto distantLight = std::static_pointer_cast<DistantLight>(light);
            pointForDepthCheck = BiasManager::getNormalAngleDistant(distantLight->shadowMap, *distantLight, normal, point, pcfKernelSize);
            biasAddition = BiasManager::getMinBias(distantLight->shadowMap);

        } else if (light->lightType == Light::LightType::SPOT) {

            auto spotLigt = std::static_pointer_cast<SpotLight>(light);
            Vector3 toL = (spotLigt->transform.getPosition() - point).normalize();
            pointForDepthCheck = BiasManager::getNormalAngleSpot(spotLigt->shadowMap, *spotLigt, normal, toL, point, pcfKernelSize);
            biasAddition = BiasManager::getMinBias(spotLigt->shadowMap);

        } else { //POINT

            auto pointLight = std::static_pointer_cast<PointLight>(light);
            Vector3 toL = (pointLight->transform.getPosition() - point).normalize();
            PointLight::ShadowMapFace face = PointLight::pickFace(point - pointLight->transform.getPosition());
            pointForDepthCheck = BiasManager::getNormalAnglePoint(*pointLight->shadowMaps[face], *pointLight, face, normal, toL, point, pcfKernelSize);

            auto faceAfter = PointLight::pickFace(pointForDepthCheck - pointLight->transform.getPosition());
            if (faceAfter != face) {
                face = faceAfter;
                pointForDepthCheck = BiasManager::getNormalAnglePoint(*pointLight->shadowMaps[face], *pointLight, face,
                                                            normal, toL, point, pcfKernelSize);
            }

            if (outFace) *outFace = face;

            biasAddition = BiasManager::getMinBias( *pointLight->shadowMaps[0]);

        }
        break;
    }

    //should be used only for Distant, if perspective light is used: fallback to Constant
    case BT::SLOPE_SCALED: {
        if (light->lightType == Light::LightType::DISTANT) {

                auto distantLight = std::static_pointer_cast<DistantLight>(light);

                auto toTexel = [&](const Vector3& w) {
                    Vector4 c = distantLight->getProjectionMatrix()*distantLight->getViewMatrix()*Vectors::vector3to4(w);
                    Vector3 ndc = {c.x/c.w, c.y/c.w, c.z/c.w};
                    return Vector3{
                        (ndc.x*0.5 + 0.5) * (distantLight->shadowMap.getCols()-1),
                        (1.0 - (ndc.y*0.5 + 0.5)) * (distantLight->shadowMap.getRows()-1),
                        (ndc.z*0.5 + 0.5)
                    };
                };


                Vector3 p0 = toTexel(fanWorldCoords.v1);
                Vector3 p1 = toTexel(fanWorldCoords.v2);
                Vector3 p2 = toTexel(fanWorldCoords.v3);

                double r = RendUt::kernelRadiusFromSide(pcfKernelSize);
                int minDim = std::min(distantLight->shadowMap.getRows(), distantLight->shadowMap.getCols());
                double alphaConst;

                //if r would be smaller than 0 we don't divide, otherwise bias would be too great
                if(pcfKernelSize <= 1){
                    alphaConst = light->bias * std::max(1, minDim);
                }else{
                    alphaConst = light->bias * std::max(1, minDim) / MathUt::safeDenom(r);
                }



                biasAddition = BiasManager::getSlopeScaled(distantLight->shadowMap, p0, p1, p2, pcfKernelSize, alphaConst);

            } else if (light->lightType == Light::LightType::POINT) {

                auto pointLight = std::static_pointer_cast<PointLight>(light);

                PointLight::ShadowMapFace face = PointLight::pickFace(point - pointLight->transform.getPosition());
                if (outFace) *outFace = face;

                auto toTexel = [&](const Vector3& w) {
                    Vector4 c = pointLight->getProjectionMatrix()*pointLight->getViewMatrix(face)*Vectors::vector3to4(w);
                    Vector3 ndc = {c.x/c.w, c.y/c.w, c.z/c.w};
                    return Vector3{
                        (ndc.x*0.5 + 0.5) * (pointLight->shadowMaps[0]->getCols()-1),
                        (1.0 - (ndc.y*0.5 + 0.5)) * (pointLight->shadowMaps[0]->getRows()-1),
                        (ndc.z*0.5 + 0.5)
                    };
                };

                Vector3 p0 = toTexel(fanWorldCoords.v1);
                Vector3 p1 = toTexel(fanWorldCoords.v2);
                Vector3 p2 = toTexel(fanWorldCoords.v3);

                double r = RendUt::kernelRadiusFromSide(pcfKernelSize);
                int minDim = std::min(pointLight->shadowMaps[face]->getRows(), pointLight->shadowMaps[face]->getCols());
                double alphaConst;

                //if r would be smaller than 0 we don't divide, otherwise bias would be too great
                if(pcfKernelSize <= 1){
                    alphaConst = light->bias * std::max(1, minDim);
                }else{
                    alphaConst = light->bias * std::max(1, minDim) / MathUt::safeDenom(r);
                }

                biasAddition = BiasManager::getSlopeScaled(*pointLight->shadowMaps[face], p0, p1, p2, pcfKernelSize, alphaConst);


            } else if (light->lightType == Light::LightType::SPOT){
                auto spotLight = std::static_pointer_cast<SpotLight>(light);
                auto toTexel = [&](const Vector3& w) {
                    Vector4 c = spotLight->getProjectionMatrix()*spotLight->getViewMatrix()*Vectors::vector3to4(w);
                    Vector3 ndc = {c.x/c.w, c.y/c.w, c.z/c.w};
                    return Vector3{
                        (ndc.x*0.5 + 0.5) * (spotLight->shadowMap.getCols()-1),
                        (1.0 - (ndc.y*0.5 + 0.5)) * (spotLight->shadowMap.getRows()-1),
                        (ndc.z*0.5 + 0.5)
                    };
                };

                Vector3 p0 = toTexel(fanWorldCoords.v1);
                Vector3 p1 = toTexel(fanWorldCoords.v2);
                Vector3 p2 = toTexel(fanWorldCoords.v3);

                double r = RendUt::kernelRadiusFromSide(pcfKernelSize);
                int minDim = std::min(spotLight->shadowMap.getRows(), spotLight->shadowMap.getCols());
                double alphaConst;

                //if r would be smaller than 0 we don't divide, otherwise bias would be too great
                if(pcfKernelSize <= 1){
                    alphaConst = light->bias * std::max(1, minDim);
                }else{
                    alphaConst = light->bias * std::max(1, minDim) / MathUt::safeDenom(r);
                }

                biasAddition = BiasManager::getSlopeScaled(spotLight->shadowMap, p0, p1, p2, pcfKernelSize, alphaConst);
                //std::cout<<"Base bias: "<<light->bias<<"  |  biasAddition: "<<biasAddition<<std::endl;
               // if(biasAddition - light->bias > 0.00001) std::cout<<"+"<<std::endl;

            }else{
                biasAddition = light->bias;
            }

        pointForDepthCheck = point;
        break;
    }

    case BT::CONSTANT:
    default:
        pointForDepthCheck = point;
        biasAddition = light->bias;
        break;
    }
}*/



double Renderer::calculateBias(const Light& light, const Vector3& worldSpacePoint, const Vector3& normal,
                               const Triangle3& fanWorldSpace, int pcfKernelSize){
    using BT = Light::BiasType;

    switch (light.biasType) {
        case BT::NORMAL_ANGLE: {
            break;
        }
    case BT::SLOPE_SCALED: {
        if (light.lightType == Light::LightType::DISTANT) {

            const DistantLight& distantLight = static_cast<const DistantLight&>(light);

            auto toTexel = [&](const Vector3& w) {
                Vector4 c = distantLight.getProjectionMatrix()*distantLight.getViewMatrix()*Vectors::vector3to4(w);
                Vector3 ndc = {c.x/c.w, c.y/c.w, c.z/c.w};
                return Vector3{
                    (ndc.x*0.5 + 0.5) * (distantLight.shadowMap.getCols()-1),
                    (1.0 - (ndc.y*0.5 + 0.5)) * (distantLight.shadowMap.getRows()-1),
                    (ndc.z*0.5 + 0.5)
                };
            };

            Vector3 p0 = toTexel(fanWorldSpace.v1);
            Vector3 p1 = toTexel(fanWorldSpace.v2);
            Vector3 p2 = toTexel(fanWorldSpace.v3);

            double r = RendUt::kernelRadiusFromSide(pcfKernelSize);
            int minDim = std::min(distantLight.shadowMap.getRows(), distantLight.shadowMap.getCols());
            double alphaConst;

            //if r would be smaller than 0 we don't divide, otherwise bias would be too great
            if(pcfKernelSize <= 1){
                alphaConst = light.bias * std::max(1, minDim);
            }else{
                alphaConst = light.bias * std::max(1, minDim) / MathUt::safeDenom(r);
            }

            double bias = BiasManager::getSlopeScaled(distantLight.shadowMap, p0, p1, p2, pcfKernelSize, alphaConst);
            return bias;

        } else if (light.lightType == Light::LightType::POINT) {

            const PointLight& pointLight = static_cast<const PointLight&>(light);

            PointLight::ShadowMapFace face = PointLight::pickFace(worldSpacePoint - pointLight.transform.getPosition());

            auto toTexel = [&](const Vector3& w) {
                Vector4 c = pointLight.getProjectionMatrix()*pointLight.getViewMatrix(face)*Vectors::vector3to4(w);
                Vector3 ndc = {c.x/c.w, c.y/c.w, c.z/c.w};
                return Vector3{
                    (ndc.x*0.5 + 0.5) * (pointLight.shadowMaps[0]->getCols()-1),
                    (1.0 - (ndc.y*0.5 + 0.5)) * (pointLight.shadowMaps[0]->getRows()-1),
                    (ndc.z*0.5 + 0.5)
                };
            };

            Vector3 p0 = toTexel(fanWorldSpace.v1);
            Vector3 p1 = toTexel(fanWorldSpace.v2);
            Vector3 p2 = toTexel(fanWorldSpace.v3);

            double r = RendUt::kernelRadiusFromSide(pcfKernelSize);
            int minDim = std::min(pointLight.shadowMaps[face]->getRows(), pointLight.shadowMaps[face]->getCols());
            double alphaConst;

            //if r would be smaller than 0 we don't divide, otherwise bias would be too great
            if(pcfKernelSize <= 1){
                alphaConst = light.bias * std::max(1, minDim);
            }else{
                alphaConst = light.bias * std::max(1, minDim) / MathUt::safeDenom(r);
            }

            double bias = BiasManager::getSlopeScaled(*pointLight.shadowMaps[face], p0, p1, p2, pcfKernelSize, alphaConst);
            return bias;

        } else if (light.lightType == Light::LightType::SPOT){
            auto spotLight = static_cast<const SpotLight&>(light);
            auto toTexel = [&](const Vector3& w) {
                Vector4 c = spotLight.getProjectionMatrix()*spotLight.getViewMatrix()*Vectors::vector3to4(w);
                Vector3 ndc = {c.x/c.w, c.y/c.w, c.z/c.w};
                return Vector3{
                    (ndc.x*0.5 + 0.5) * (spotLight.shadowMap.getCols()-1),
                    (1.0 - (ndc.y*0.5 + 0.5)) * (spotLight.shadowMap.getRows()-1),
                    (ndc.z*0.5 + 0.5)
                };
            };

            Vector3 p0 = toTexel(fanWorldSpace.v1);
            Vector3 p1 = toTexel(fanWorldSpace.v2);
            Vector3 p2 = toTexel(fanWorldSpace.v3);

            double r = RendUt::kernelRadiusFromSide(pcfKernelSize);
            int minDim = std::min(spotLight.shadowMap.getRows(), spotLight.shadowMap.getCols());
            double alphaConst;

            //if r would be smaller than 0 we don't divide, otherwise bias would be too great
            if(pcfKernelSize <= 1){
                alphaConst = light.bias * std::max(1, minDim);
            }else{
                alphaConst = light.bias * std::max(1, minDim) / MathUt::safeDenom(r);
            }


            double bias = BiasManager::getSlopeScaled(spotLight.shadowMap, p0, p1, p2, pcfKernelSize, alphaConst);
            return bias;

        }else{
            return light.bias;
        }
        break;
    }

    case BT::CONSTANT:
    default:
        return light.bias;
        break;
    }
}

/*
Renderer::Bias Renderer::calculateBias(const std::shared_ptr<Light>& light, const Vector3& point, const Vector3& normal,
                   Triangle3& fanWorldCoords, PointLight::ShadowMapFace face, int pcfKernelSize){


    using BT = Light::BiasType;
    Bias result;
    result.biasAddition = light->bias;
    result.pointForDepthCheck = point;
    result.face = face;

    switch (light->biasType) {
    case BT::NORMAL_ANGLE: {
        if (light->lightType == Light::LightType::DISTANT) {

            auto distantLight = std::static_pointer_cast<DistantLight>(light);
            result.pointForDepthCheck = BiasManager::getNormalAngleDistant(distantLight->shadowMap, *distantLight, normal, point, pcfKernelSize);

        } else if (light->lightType == Light::LightType::SPOT) {

            auto spotLigt = std::static_pointer_cast<SpotLight>(light);
            Vector3 toL = (spotLigt->transform.getPosition() - point).normalize();
            result.pointForDepthCheck = BiasManager::getNormalAngleSpot(spotLigt->shadowMap, *spotLigt, normal, toL, point, pcfKernelSize);

        } else { //POINT

            auto pointLight = std::static_pointer_cast<PointLight>(light);
            Vector3 toL = (pointLight->transform.getPosition() - point).normalize();
            result.pointForDepthCheck = BiasManager::getNormalAnglePoint(*pointLight->shadowMaps[face], *pointLight, face, normal, toL, point, pcfKernelSize);

            auto faceAfter = PointLight::pickFace(result.pointForDepthCheck - pointLight->transform.getPosition());

            if (faceAfter != result.face) {
                result.face = faceAfter;
                result.pointForDepthCheck = BiasManager::getNormalAnglePoint(*pointLight->shadowMaps[face], *pointLight, face,
                                                                      normal, toL, point, pcfKernelSize);
            }
        }
        break;
    }

    //should be used only for Distant, if perspective light is used: fallback to Constant
    case BT::SLOPE_SCALED: {
        if (light->lightType == Light::LightType::DISTANT) {

            auto distantLight = std::static_pointer_cast<DistantLight>(light);
            Vector3 p0 = Vectors::vector4to3(distantLight->getViewMatrix() * Vectors::vector3to4(fanWorldCoords.v1)),
                p1 = Vectors::vector4to3(distantLight->getViewMatrix() * Vectors::vector3to4(fanWorldCoords.v2)),
                p2 = Vectors::vector4to3(distantLight->getViewMatrix() * Vectors::vector3to4(fanWorldCoords.v3));


            auto eff = (pcfKernelSize <= 1) ? 1.0 : double((pcfKernelSize - 1) / 2);
            int minDim = std::min(distantLight->shadowMap.getRows(), distantLight->shadowMap.getCols());
            double alphaConst = light->bias * std::max(1, minDim) / std::max(1e-6, eff);

            result.biasAddition = BiasManager::getSlopeScaled(distantLight->shadowMap, p0, p1, p2, pcfKernelSize, alphaConst);

            // std::cout<<"Bias addition : "<<biasAddition<<"|  bias: "<<distantLight->bias<<std::endl;

        } else {

            result.biasAddition = light->bias;
        }

        result.pointForDepthCheck = point;
        break;
    }

    case BT::CONSTANT:
    default:
        result.pointForDepthCheck = point;
        result.biasAddition = light->bias;
        break;
    }
}
*/

double Renderer::calculateShadowAmount(const Buffer<double>& shadowMap,const Light& light, const Vector2& point,
                             double receiverDepth, double bias ){

    using FT = Light::FilteringType;
    switch (light.filteringType){
    case FT::NONE: {
        const int sx = std::clamp((int)std::round(point.x), 0, (int)shadowMap.getCols()-1);
        const int sy = std::clamp((int)std::round(point.y), 0, (int)shadowMap.getRows()-1);
        return (shadowMap[sy][sx] + bias < receiverDepth) ? 1.0 : 0.0;
    }
    case FT::BILINEAR:
        return FilteringManager::bilinearFiltering(shadowMap, point, receiverDepth, bias);
    case FT::PCF_3x3:
        return FilteringManager::pcf3x3(shadowMap, point, receiverDepth, bias);
    case FT::PCF_5x5:
        return FilteringManager::pcf5x5(shadowMap, point, receiverDepth, bias);
    case FT::PCF_POISSON:
        return FilteringManager::pcfPoisson(shadowMap, point, receiverDepth, bias,
                                            light.pcfPoissonSamples, light.pcfPoissonRadiusTexels);
    case FT::PCSS:
        return FilteringManager::pcssPoisson(shadowMap, point, receiverDepth, bias, light);
    default:
        return FilteringManager::pcf5x5(shadowMap, point, receiverDepth, bias);
    }
}

void Renderer::clearRenderingSurface(){
    pixelPainter->fillImage(Colors::Black);
}

void Renderer::setRenderingSurface(std::shared_ptr<RenderingSurface> newRenderingSurface){
    renderingSurface = newRenderingSurface;
}

void Renderer::setScene(std::shared_ptr<Scene> newScene){
    scene = newScene;
}

void Renderer::setCamera(std::shared_ptr<Camera> newCamera){
    camera = newCamera;
}

std::shared_ptr<Camera> Renderer::getCamera(){
    return camera;
}

std::shared_ptr<Scene> Renderer::getScene(){
    return scene;
}

std::shared_ptr<RenderingSurface> Renderer::getRenderingSurface(){
    return renderingSurface;
}

std::shared_ptr<Buffer<IdBufferElement>> Renderer::getIdBuffer(){
    return idBuffer;
}

Vector4 Renderer::modelToClip(const Vector3& v, const Matrix4& modelMatrix){
    Vector4 result = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix * Vector4(v.x,v.y,v.z, 1.0);
    return result;
}

Vector4 Renderer::worldToClip(const Vector3& v){
    Vector4 result = camera->getProjectionMatrix() * camera->getViewMatrix() * Vector4(v.x,v.y,v.z, 1.0);
    return result;
}

Vector3 Renderer::modelToWorld(const Vector3& v, const Matrix4& modelMatrix){
    Vector4 resultHom = modelMatrix * Vector4(v.x,v.y,v.z, 1.0);
    Vector3 result = Vector3(resultHom.x, resultHom.y, resultHom.z);
    return result;
}

Vector3 Renderer::worldToCamera(const Vector3& v){
    Vector4 resultHom = camera->getViewMatrix() * Vector4(v.x,v.y,v.z, 1.0);
    Vector3 result = Vector3(resultHom.x, resultHom.y, resultHom.z);
    return result;
}

Vector3 Renderer::clipToNdc(const Vector4& v){
    return Vector3(v.x/v.w, v.y/v.w, v.z/v.w);
}

Vector2 Renderer::ndcToScreen(const Vector3& v){
    return Vector2(
        (v.x + 1) * 0.5 * renderingSurface->getImg()->width(),
        (1 - (v.y + 1)*0.5) * renderingSurface->getImg()->height());
}

Vector3 Renderer::getFaceNormal(Vector3 v0, Vector3 v1, Vector3 v2, bool inverse){
    double modifier = 0.0;
    inverse? modifier=-1.0 : modifier=1.0;

    return Vector3( ((v1-v0).crossProduct(v2-v0))*modifier );
}

Vector3 Renderer::normalToWorld(Transform3D objTransform, const Vector3& normal){
    return ((Matrices4::Matrix4To3(objTransform.getTransMatrix()).getInversion().transpose())*normal).normalize();
}

std::pair<int,int> Renderer::ndcToShadowMapTexel(const Vector2& ndc, const Buffer<double>& shadowMap){
    std::pair<int,int> result;
    result.first = std::clamp(int((ndc.x*0.5 + 0.5) * (shadowMap.getCols()-1) + 0.5), 0, int(shadowMap.getCols()-1));
    result.second = std::clamp(int((1.0 - (ndc.y*0.5 + 0.5)) * (shadowMap.getRows()-1) + 0.5), 0, int(shadowMap.getRows()-1));
    return result;
}

void Renderer::clearBuffers(){
    zBuffer->clear();
    idBuffer->clear();
}

void Renderer::updateCommonMatrices(){
    viewMatrix = camera->getViewMatrix();
    ProjectionMatrix = camera->getProjectionMatrix();
    viewProjectionMatrix = ProjectionMatrix * viewMatrix;
}





