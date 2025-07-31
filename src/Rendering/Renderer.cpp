#include "Rendering/Renderer.h"
#include "Math/Vectors.h"
#include "Scene/DistantLight.h"
#include "Scene/SpotLight.h"
#include "Scene/PointLight.h"


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
    linePainter = std::make_shared<LinePainter>(renderingSurface->getImg());
    clippingManager = std::make_shared<ClippingManager>();
    shadingManager = std::make_shared<ShadingManager>();
}

void Renderer::renderScene(){
    clearBuffers();
    updateCommonMatrices();
    updateShadowMaps();
    renderSceneObjects();
}

void Renderer::renderSceneObjects(){

    LinePainter linePainter = LinePainter(renderingSurface->getImg());

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

    if(debugMode == DEBUG_SHADOWMAP){
        std::cout<<"\n\n\n===============================NEW FRAME========================\n\n"<<std::endl;
    }

    // preprocess
    const int width  = getRenderingSurface()->getImg()->width();
    const int height = getRenderingSurface()->getImg()->height();
    const Matrix4 M = obj.transform.getTransMatrix();
    const Vector3 camPos = getCamera()->transform.getPosition();
    const Color baseColor = obj.viewportDisplay.color;
    Color finalColor = baseColor;

    //building clip-space for all vertices and depending on shading, getting color on vertices
    std::vector<ClippingManager::ClippedVertex> clipVertices;
    for(size_t i = 0; i < obj.vertices.size(); i++){

        const Vector3& localSpaceVertex = obj.vertices[i];

        Vector4 clipSpaceVertex = modelToClip(localSpaceVertex , M);
        double invertedW = 1.0/clipSpaceVertex.w;
        Vector3 worldSpaceVertex = modelToWorld(localSpaceVertex, M);
        Vector3 worldSpaceNormal = normalToWorld(obj.transform , obj.vertexNormals[i]);

        //calculate light at vertices, used in Gourand shading
        Vector3 vertColor;
        switch(obj.displaySettings->lightingMode){
        case DisplaySettings::LightingModel::FACE_RATIO:
            vertColor = Vectors::colorToVector3(shadingManager->shadeColorFR(
                camPos,
                worldSpaceVertex,
                worldSpaceNormal,
                baseColor
                ));
            break;
        case DisplaySettings::LightingModel::LAMBERT:
            break;
        case DisplaySettings::LightingModel::NONE:
            break;
        }

        clipVertices.push_back({
            clipSpaceVertex,
            invertedW,
            worldSpaceVertex * invertedW,
            worldSpaceNormal * invertedW,
            vertColor * invertedW
        });
    }

    //iterate through every face
    for (size_t face = 0; face < obj.faceVertexIndices.size(); face += 3)
    {

        //original vertices in clip-space
        Triangle<ClippingManager::ClippedVertex> clipSpaceTriangle(clipVertices[obj.faceVertexIndices[face    ]],
                                                               clipVertices[obj.faceVertexIndices[face + 1]],
                                                               clipVertices[obj.faceVertexIndices[face + 2]]);

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
            double invArea = 1.0 / area;

            //if flat shading is used we calculateColor once using normal at middle of triangle
            if(obj.displaySettings->lightingMode == DisplaySettings::LightingModel::FACE_RATIO &&
                obj.displaySettings->shadingMode == DisplaySettings::Shading::FLAT){

                int middleX = minX + (maxX - minX)/2;
                int middleY = minY + (maxY - minY)/2;

                //interpolate middle of the triangle for color
                Triangle<double> baricentricFactor = fanTriangleScreenSpace.baricentricCoords(Vector2(middleX, middleY));
                double invDenom = baricentricFactor.v1 * fanTriangleClipped.v1.invW
                                  + baricentricFactor.v2 * fanTriangleClipped.v2.invW
                                  + baricentricFactor.v3 * fanTriangleClipped.v3.invW;

                Vector3 interpolatedWorldSpaceCoords = (fanTriangleClipped.v1.worldSpaceVertexOverW*baricentricFactor.v1
                                                        + fanTriangleClipped.v2.worldSpaceVertexOverW*baricentricFactor.v2
                                                        + fanTriangleClipped.v3.worldSpaceVertexOverW*baricentricFactor.v3)/invDenom;

                Vector3 interpolatedWorldSpaceFaceNormal = ((fanTriangleClipped.v1.worldSpaceNormalOverW*baricentricFactor.v1
                                                             + fanTriangleClipped.v2.worldSpaceNormalOverW*baricentricFactor.v2
                                                             + fanTriangleClipped.v3.worldSpaceNormalOverW*baricentricFactor.v3)
                                                            /invDenom).normalize();


                finalColor = shadingManager.get()->shadeColorFR(
                    getCamera()->transform.getPosition(),
                    interpolatedWorldSpaceCoords, interpolatedWorldSpaceFaceNormal, baseColor);
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

                        if(obj.displaySettings->lightingMode == DisplaySettings::LightingModel::FACE_RATIO &&
                            obj.displaySettings->shadingMode == DisplaySettings::Shading::PHONG){
                            double invDenom = baricentricFactor.v1 * fanTriangleClipped.v1.invW
                                              + baricentricFactor.v2 * fanTriangleClipped.v2.invW
                                              + baricentricFactor.v3 * fanTriangleClipped.v3.invW;

                            Vector3 interpolatedWorldSpaceCoords = (fanTriangleClipped.v1.worldSpaceVertexOverW*baricentricFactor.v1
                                                                    + fanTriangleClipped.v2.worldSpaceVertexOverW*baricentricFactor.v2
                                                                    + fanTriangleClipped.v3.worldSpaceVertexOverW*baricentricFactor.v3)/invDenom;

                            Vector3 interpolatedWorldSpaceFaceNormal = ((fanTriangleClipped.v1.worldSpaceNormalOverW*baricentricFactor.v1
                                                                         + fanTriangleClipped.v2.worldSpaceNormalOverW*baricentricFactor.v2
                                                                         + fanTriangleClipped.v3.worldSpaceNormalOverW*baricentricFactor.v3)
                                                                        /invDenom).normalize();

                            //shade check
                            const double SHADOW_INTENSITY = 0.2;
                            double lightMultiplier = 1;
                            /*
                            if(debugMode == DEBUG_SHADOWMAP){
                                std::cout<<std::endl<<"Debug pixel [screen space]: "<<x<<":"<<y<<std::endl;
                            }*/
                            for(std::shared_ptr<Light>lightSource : scene->lightSources){
                                //cast Light to its proper subclass
                                if(lightSource->lightType == Light::LightType::DISTANT){
                                    if(auto distantLight = std::dynamic_pointer_cast<DistantLight>(lightSource)){

                                        bool isInShadow = true;
                                        Vector4 lightProjCoord = distantLight->getProjectionMatrix() * distantLight->getViewMatrix() * Vectors::vector3to4(interpolatedWorldSpaceCoords);
                                        Vector3 lightNdcCoord = Vector3(lightProjCoord.x/lightProjCoord.w, lightProjCoord.y/lightProjCoord.w, lightProjCoord.z/lightProjCoord.w);


                                        //check if we are not trying to get element from outside the buffer
                                        float u =  lightNdcCoord.x * 0.5f + 0.5f;       // 0 … 1
                                        float v = 1.0f - (lightNdcCoord.y * 0.5f + 0.5f);



                                        //if something is outside shadowMap then it's definitely not in shadow
                                        if (std::isfinite(u) && std::isfinite(v) && ((u >= 0.0f && u < 1.0f) && (v >= 0.0f && v < 1.0f))){

                                            float depthInLightView = lightNdcCoord.z * 0.5f + 0.5f;
                                            int sx = int(std::round((lightNdcCoord.x * 0.5f + 0.5f) * (distantLight->shadowMap.getCols() - 1)));
                                            int sy = int(std::round((1 - (lightNdcCoord.y * 0.5f + 0.5f)) * (distantLight->shadowMap.getRows() - 1)));

                                            if (debugMode == DEBUG_SHADOWMAP) {
                                                //  std::cout << "u: " << u << " v: " << v << " sx: " << sx << " sy: " << sy
                                                //           << " depthInLightView: " << depthInLightView
                                                //   << " sm: " << distantLight->shadowMap[sy][sx] << std::endl;
                                            }//

                                            if (depthInLightView <= distantLight->shadowMap[sy][sx]){
                                                if (debugMode == DEBUG_SHADOWMAP) {
                                                    //  std::cout << "pixel not in shadow, because of depth check"<<std::endl;
                                                }
                                                isInShadow = false;
                                            }
                                        }else{
                                            if (debugMode == DEBUG_SHADOWMAP) {
                                                // std::cout << "pixel outside of shadowMap"<<std::endl;
                                            }
                                            isInShadow = false;
                                        }

                                        if(!isInShadow){
                                            // if (debugMode == DEBUG_SHADOWMAP) std::cout<<"final verdict: Pixel Illuminated"<<std::endl;
                                            lightMultiplier = lightMultiplier * shadingManager->getReflectedLightLambert(
                                                                  distantLight->direction, interpolatedWorldSpaceFaceNormal, distantLight->intensity
                                                                  );
                                            //finalColor = Colors::White; //white - light
                                        }else{
                                            //finalColor = Colors::Red; //red - shadow
                                            lightMultiplier = SHADOW_INTENSITY;
                                            if (debugMode == DEBUG_SHADOWMAP){
                                                //std::cout<<"final verdict: Pixel in Shadow"<<std::endl;
                                            }
                                        }
                                    }
                                }else if(lightSource->lightType == Light::LightType::POINT){
                                    if(auto pointLight = std::dynamic_pointer_cast<PointLight>(lightSource)){

                                        //checking which face should be checked for shadows
                                        Vector3 lightVector = interpolatedWorldSpaceCoords - pointLight->transform.getPosition();
                                        double absX = std::fabs(lightVector.x), absY = std::fabs(lightVector.y), absZ = std::fabs(lightVector.z);
                                        int face = 0;
                                        if(absX >= absY && absX >= absZ){
                                            if(lightVector.x >= 0){
                                                face = 0;
                                            }else{
                                                face = 1;
                                            }
                                        }else if(absY >= absX && absY >= absZ){
                                            if(lightVector.y >= 0){
                                                face = 2;
                                            }else{
                                                face = 3;
                                            }
                                        }else if(absZ >= absX && absZ >= absY){
                                            if(lightVector.z >= 0){
                                                face = 4;
                                            }else{
                                                face = 5;
                                            }
                                        }

                                        bool isInShadow = true;
                                        Vector4 lightProjCoord = pointLight->getProjectionMatrix() * pointLight->getViewMatrix(face) * Vectors::vector3to4(interpolatedWorldSpaceCoords);
                                        Vector3 lightNdcCoord = Vector3(lightProjCoord.x/lightProjCoord.w, lightProjCoord.y/lightProjCoord.w, lightProjCoord.z/lightProjCoord.w);

                                        //check if we are not trying to get element from outside the buffer
                                        float u =  lightNdcCoord.x * 0.5f + 0.5f;       // 0 … 1
                                        float v = 1.0f - (lightNdcCoord.y * 0.5f + 0.5f);

                                        //if something is outside shadowMap then it's definitely not in shadow
                                        if (std::isfinite(u) && std::isfinite(v) && ((u >= 0.0f && u < 1.0f) && (v >= 0.0f && v < 1.0f))){

                                                // float depthInLightView = lightNdcCoord.z * 0.5f + 0.5f;
                                            float depthInLightView = (interpolatedWorldSpaceCoords - lightSource->transform.getPosition()).length();

                                            int sx = int(std::round((lightNdcCoord.x * 0.5f + 0.5f) * (pointLight->shadowMaps[face]->getCols() - 1)));
                                            int sy = int(std::round((1 - (lightNdcCoord.y * 0.5f + 0.5f)) * (pointLight->shadowMaps[face]->getRows() - 1)));

                                            if (debugMode == DEBUG_SHADOWMAP) {/*
                                                std::cout << "u: " << u << " v: " << v << " sx: " << sx << " sy: " << sy
                                                            << " depthInLightView: " << depthInLightView
                                                          << " sm: " << (*pointLight->shadowMaps[face])[sy][sx] << std::endl;
                                                */}

                                            if (depthInLightView <= (*pointLight->shadowMaps[face])[sy][sx]){
                                                if (debugMode == DEBUG_SHADOWMAP) {
                                                    // std::cout << "pixel not in shadow, because of depth check"<<std::endl;
                                                }
                                                isInShadow = false;
                                            }
                                        }else{
                                            if (debugMode == DEBUG_SHADOWMAP) {
                                                // std::cout << "pixel outside of shadowMap"<<std::endl;
                                            }
                                            isInShadow = false;
                                        }

                                        if(!isInShadow){
                                            Vector3 lightDirection = (interpolatedWorldSpaceCoords - pointLight->transform.getPosition());
                                            double lightToPixelDistance = lightDirection.length();
                                            double lightAttenuation = pointLight->getAttenuation(lightToPixelDistance);
                                            Vector3 normalizedLightDirection = lightDirection.normalize()*(-1.0);
                                            //if (debugMode == DEBUG_SHADOWMAP) std::cout<<"final verdict: Pixel Illuminated"<<std::endl;
                                            lightMultiplier = lightMultiplier * shadingManager->getReflectedLightLambert(
                                                                  normalizedLightDirection, interpolatedWorldSpaceFaceNormal, pointLight->intensity
                                                                  ) ;
                                            std::cout<<"Light mult: "<<lightMultiplier<<std::endl;
                                            lightMultiplier*=lightAttenuation;
                                            std::cout<<"Light mult, after att: "<<lightMultiplier<<std::endl;
                                            if (debugMode == DEBUG_SHADOWMAP) std::cout<<"final verdict: Pixel Illuminated, lightMult: "<< lightMultiplier<<std::endl;
                                            //finalColor = Colors::White; //white - light
                                        }else{
                                            //finalColor = Colors::Red; //red - shadow
                                            lightMultiplier = SHADOW_INTENSITY;
                                            if (debugMode == DEBUG_SHADOWMAP){
                                                //  std::cout<<"final verdict: Pixel in Shadow"<<std::endl;
                                            }
                                        }
                                    }
                                }
                            }
                            finalColor = baseColor * lightMultiplier;
                        }else if(obj.displaySettings->lightingMode == DisplaySettings::LightingModel::FACE_RATIO &&
                                   obj.displaySettings->shadingMode == DisplaySettings::Shading::GOURAUD){

                            double invDenom = baricentricFactor.v1 * fanTriangleClipped.v1.invW
                                              + baricentricFactor.v2 * fanTriangleClipped.v2.invW
                                              + baricentricFactor.v3 * fanTriangleClipped.v3.invW;
                            Vector3 interpolatedColor = (fanTriangleClipped.v1.colorOverW*baricentricFactor.v1
                                                         + fanTriangleClipped.v2.colorOverW*baricentricFactor.v2
                                                         + fanTriangleClipped.v3.colorOverW*baricentricFactor.v3)/invDenom;
                            finalColor = Vectors::vector3ToColor(interpolatedColor);
                        }

                        if (paintTool.drawPixel(x, y, depth, finalColor)) {
                            IdBufferElement fillEl;
                            fillEl.objectId     = objId;
                            fillEl.faceId       = int(face/3);
                            (*idBuffer)[y][x] = fillEl;
                        }
                    }
                }
            }
        }

        //pass wireframe - drawing edges of rasterizatied triangle
        if(obj.displaySettings->colorWireframes || obj.displaySettings->renderMode == DisplaySettings::RenderMode::WIREFRAME){

            IdBufferElement edgeEl;
            edgeEl.objectId = objId;
            edgeEl.faceId   = int(face/3);
            size_t facePolysAmount = screenVerticesWithDepth.size();

            for (size_t i = 0; i < facePolysAmount; i++) {
                Vector3 edgeVertex1 = screenVerticesWithDepth[i];
                Vector3 edgeVertex2 = screenVerticesWithDepth[(i+1)%facePolysAmount];

                edgeVertex1.z = std::max(0.0, edgeVertex1.z - doubleBias);
                edgeVertex2.z = std::max(0.0, edgeVertex2.z - doubleBias);

                int idxA = (i   < 3 ? obj.faceVertexIndices[face+i  ] : -1);
                int idxB = (i+1 < 3 ? obj.faceVertexIndices[face+i+1] : -1);
                if (i == 2) idxB = obj.faceVertexIndices[face];

                edgeEl.edgeVertices = { idxA, idxB };
                paintTool.drawLine3D(edgeVertex1, edgeVertex2, edgeEl,
                                     obj.displaySettings->colorWireframes? obj.viewportDisplay.wireframeColor : obj.viewportDisplay.color);
            }
        }
    }
}

void Renderer::updateShadowMaps(){

    if(debugMode == DEBUG_SHADOWMAP){
        std::cout << "started updateShadowMaps, number of light sources on the scene: "<<scene->lightSources.size()<<std::endl;
    }

    std::vector<Vector3> cameraFrustum = camera->getFrustum();

    //filling shadow maps of light sources
    for(std::shared_ptr<Light>lightSource : scene->lightSources){
        if(debugMode == DEBUG_SHADOWMAP){
            //   std::cout << "if start" <<std::endl;
        }
        //cast Light to its proper subclass
        if(lightSource->lightType == Light::LightType::DISTANT){
            if(debugMode == DEBUG_SHADOWMAP){
                std::cout << "Found distant light source"<<std::endl;
            }
            if(auto distantLight = std::dynamic_pointer_cast<DistantLight>(lightSource)){


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
                lightView = backOff * lightView;

                // calculate bbox Z after backOff
                for (auto& v : cameraFrustum) {
                    Vector3 inLS = Vectors::vector4to3(lightView * Vectors::vector3to4(v));
                    minZ = std::min(minZ, inLS.z);
                    maxZ = std::max(maxZ, inLS.z);
                }

                // calculate near and far and set proj matrix
                double nearDist = 0.1;
                double farDist  = (-minZ) + padZ;

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
                        Vector4 proj = lightProjection * lightView * v4;
                        Vector3 ndc = Vector3(proj.x / proj.w, proj.y / proj.w, proj.z / proj.w);
                        std::cout << "NDC: " << ndc << std::endl;
                    }
                }
                shadowMapDepthPass((*distantLight), lightView, lightProjection);
                //depth pass - filling shadow map of current light source

            }
        }else if(lightSource->lightType == Light::LightType::POINT){
            if(debugMode == DEBUG_SHADOWMAP){
                std::cout << "Found point light source"<<std::endl;
            }
            if(std::shared_ptr<PointLight> pointLight = std::dynamic_pointer_cast<PointLight>(lightSource)){


                //get light view matrices
                pointLight->setViewMatrices();

                double near = 0.1;
                double far = pointLight->range;

                pointLight->setProjectionMatrix(near, far);
                if(debugMode == DEBUG_SHADOWMAP){
                    std::cout << "perspective projection:\n" << pointLight->getProjectionMatrix() << std::endl;
                }

                shadowMapDepthPass((*pointLight));


            }

        }else if(lightSource->lightType == Light::LightType::SPOT){
            if(debugMode == DEBUG_SHADOWMAP){
                std::cout << "somehow found spot light source"<<std::endl;
            }
            if(auto pointLight = std::dynamic_pointer_cast<SpotLight>(lightSource)){


            }
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

void Renderer::shadowMapDepthPass(DistantLight& lightSource, const Matrix4& lightView, const Matrix4& lightProjection){
    //clear shadowMap
    lightSource.shadowMap.clear();

    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){
        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){
            if(curObject->displaySettings->renderMode == DisplaySettings::RenderMode::NONE) continue;

            Matrix4 MVP = lightProjection * lightView * curObject->transform.getTransMatrix();

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
                    Vector2 screenSpaceVertex = Vector2(int(std::round( (normalizedClipVertex.x * 0.5 + 0.5) * (lightSource.shadowMap.getCols()-1) ) ) ,
                                                        std::round(int( (1 - (normalizedClipVertex.y*0.5+0.5)) * (lightSource.shadowMap.getRows()-1) )));
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


                        Vector2 screenSpaceVertex = Vector2(int(std::round( (normalizedClipVertex.x * 0.5 + 0.5) * (lightSource.shadowMaps[shadowCubeFace]->getCols()-1) ) ) ,
                                                            std::round(int( (1 - (normalizedClipVertex.y*0.5+0.5)) * (lightSource.shadowMaps[shadowCubeFace]->getRows()-1) )));

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
                                if (depthInLightView < (*lightSource.shadowMaps[shadowCubeFace])[y][x]){
                                    (*lightSource.shadowMaps[shadowCubeFace])[y][x] = depthInLightView;
                                    (*lightSource.shadowMaps[shadowCubeFace])[y][x] = std::min(lightSource.range, (*lightSource.shadowMaps[shadowCubeFace])[y][x]);
                                }
                            }
                        }

                    }
                }
            }

        }
    }

    if(debugMode == DEBUG_SHADOWMAP){

        lightSource.printShadowMaps();
        for(int face = 0; face<6; face++){
            std::cout<<"ShadowMap "<<face<<" non-empty elements: "<<lightSource.shadowMaps[face]->nonEmptyElements()<<"//"<<lightSource.shadowMaps[face]->size()<<std::endl;
            std::cout<<"lightView matrix "<<face<<" :\n"<<lightSource.getViewMatrix(face)<<std::endl;
        }
        //lightSource.shadowMap.logNonEmptyElements();

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


void Renderer::clearBuffers(){
    zBuffer->clear();
    idBuffer->clear();
}

void Renderer::updateCommonMatrices(){
    viewMatrix = camera->getViewMatrix();
    ProjectionMatrix = camera->getProjectionMatrix();
    viewProjectionMatrix = ProjectionMatrix * viewMatrix;
}





