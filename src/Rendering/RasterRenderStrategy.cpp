#include "Rendering/RasterRenderStrategy.h"
#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/RenderableObject3D.h"

/* --+-- Lasciate ogni speranza, voi ch'entrate --+-- */
void RasterRenderStrategy::render(RenderableObject3D& obj,
                                  Renderer& renderer,
                                  int objId)
{
    //preparation / preprocess
    const int width  = renderer.getRenderingSurface()->getImg()->width();
    const int height = renderer.getRenderingSurface()->getImg()->height();
    const Matrix4 M = obj.transform.getTransMatrix();
    Color baseColor = obj.viewportDisplay.color;

    //building clip-space for all vertices
    std::vector<ClippingManager::ClippedVertex> clipVertices;
    std::vector<Vector2> uv;
    for(size_t i = 0; i < obj.vertices.size(); i++){

        const Vector3& localSpaceVertex = obj.vertices[i];

        Vector4 clipSpaceVertex = renderer.modelToClip(localSpaceVertex , M);
        double invertedW = 1.0/clipSpaceVertex.w;
        Vector3 worldSpaceVertex = renderer.modelToWorld(localSpaceVertex, M);
        Vector3 worldSpaceFaceNormal = renderer.faceNormalToWorld(obj.transform , obj.vertexNormals[i]);

        clipVertices.push_back({
            clipSpaceVertex,
            invertedW,
            worldSpaceVertex * invertedW,
            worldSpaceFaceNormal * invertedW
        });
    }

    /*
    for (auto& v : obj.vertices){
        clip.push_back(renderer.modelToClip(v, M));
        uv.push_back(obj.textureCoords[ &v - &obj.vertices[0] ]);
    }


    //world space vertices are used in smooth shading
    std::vector<Vector3>worldSpaceVertices(obj.vertices.size());
    for(auto& v : obj.vertices){
        worldSpaceVertices.push_back(renderer.modelToWorld(v , obj.transform.getTransMatrix()));
    }

    std::vector<Vector3> transformedNormals;
    for(auto& n : obj.normals){
        transformedNormals.push_back(renderer.faceNormalToWorld(obj.transform, n));

    }

    std::vector<Vector3> transformedVertexNormals;
    for(auto&n : obj.vertexNormals){
        transformedVertexNormals.push_back(renderer.faceNormalToWorld(obj.transform, n));
    }
    */
    //iterate through every face
    for (size_t face = 0; face < obj.faceVertexIndices.size(); face += 3)
    {
        //parameters for simple shading
        /*
        Vector3 normal = transformedNormals[face/3];
        Vector3 faceCenterWorldSpace = (renderer.modelToWorld(obj.vertices[obj.faceVertexIndices[face]] , obj.transform.getTransMatrix()) +
                                        renderer.modelToWorld(obj.vertices[obj.faceVertexIndices[face+1]] , obj.transform.getTransMatrix()) +
                                        renderer.modelToWorld(obj.vertices[obj.faceVertexIndices[face+2]] , obj.transform.getTransMatrix()))*(1.0/3.0);

        Color shadedColor = renderer.shadingManager.get()->shadeColorFR(renderer.getCamera()->transform.getPosition(),
                                                                        faceCenterWorldSpace,normal,baseColor);
        */

        //original vertices in clip-space
        ClippingManager::ClippedVertex cv0 = clipVertices[obj.faceVertexIndices[face    ]];
        ClippingManager::ClippedVertex cv1 = clipVertices[obj.faceVertexIndices[face + 1]];
        ClippingManager::ClippedVertex cv2 = clipVertices[obj.faceVertexIndices[face + 2]];

        //clipping triangle
        std::vector<ClippingManager::ClippedVertex> clippedPoly = renderer.clippingManager->clipTriangle({cv0, cv1, cv2});
        if (clippedPoly.size() < 3) continue;


        // clip -> ndc -> screen - > screen with normalized depth
        std::vector<Vector3> screenVerticesWithDepth;
        screenVerticesWithDepth.reserve(clippedPoly.size());
        for (ClippingManager::ClippedVertex& cv : clippedPoly){
            Vector3 normalizedClipVertex = renderer.clipToNdc(cv.clip);
            Vector2 screenSpaceVertex = renderer.ndcToScreen(normalizedClipVertex);
            screenVerticesWithDepth.push_back(Vector3(screenSpaceVertex.x , screenSpaceVertex.y, ((cv.clip.z/cv.clip.w)+1.0)*0.5));
        }

        // ndc -> screen
        /*
        screenVertices.reserve(normalizedVertices.size());
        for (auto& nv : normalizedVertices)
            screenVertices.push_back(renderer.ndcToScreen(nv));
        */

        // preparing data
        // screenDepthVertices = {screen.x , screen.y, normalized.z}; normalization in range [0,1]
        /*
        std::vector<Vector3> screenDepthVertices;
        std::vector<double> clipZ;
        std::vector<double> invW;
        std::vector<double> u_over_w, v_over_w;
        screenDepthVertices.reserve(clippedPoly.size());

        for (size_t i = 0; i < clippedPoly.size(); ++i) {
            auto& sv = screenVertices[i];
            auto& cv = clippedPoly[i];

            clipZ.push_back(cv.z);
            invW.push_back(1.0 / cv.w);
            u_over_w.push_back( obj.textureCoords[obj.faceVertexIndices[face + i]].x * invW.back() );
            v_over_w.push_back( obj.textureCoords[obj.faceVertexIndices[face + i]].y * invW.back() );

            double normalizedDepth = ((cv.z/cv.w)+1.0)*0.5;
            screenDepthVertices.push_back({ sv.x, sv.y, normalizedDepth });
        }
        */


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
            int maxX = std::min(width-1, int(std::ceil(std::max({A.x,B.x,C.x}))));
            int minY = std::max(0, int(std::floor(std::min({A.y,B.y,C.y}))));
            int maxY = std::min(height-1, int(std::ceil(std::max({A.y,B.y,C.y}))));

            //calculating area of triangle
            //if triangle's area = 0, nothing to raster
            //inv area will be used in further calculations
            double area = (B.x - A.x)*(C.y - A.y)
                          - (B.y - A.y)*(C.x - A.x);
            if (area == 0.0) continue;
            double invArea = 1.0 / area;

            // raster fill
            for (int y = minY; y <= maxY; y++) {
                for (int x = minX; x <= maxX; x++) {

                    double px = x + 0.5, py = y + 0.5; //middle of current pixel

                    //checking if pixel is withing triangle using baricentric coordinates
                    double w0 = ((B.x-px)*(C.y-py) - (B.y-py)*(C.x-px)) * invArea;
                    double w1 = ((C.x-px)*(A.y-py) - (C.y-py)*(A.x-px)) * invArea;
                    double w2 = 1.0 - w0 - w1;
                    if (w0<0 || w1<0 || w2<0) continue;

                    //depth interpolation / perspective-correct interpolation
                    /*
                    double num   = w0 * clipZ[0] * invW[0]
                                 + w1 * clipZ[k] * invW[k]
                                 + w2 * clipZ[k+1] * invW[k+1];

                    double denom = w0 * invW[0]
                                   + w1 * invW[k]
                                   + w2 * invW[k+1];
                    */

                    double invDenom = w0 * v0.invW + w1 * v1.invW + w2 * v2.invW;

                    Vector3 interpolatedWorldSpaceCoords = (v0.worldSpaceVertexOverW*w0 + v1.worldSpaceVertexOverW*w1 + v2.worldSpaceVertexOverW*w2)/invDenom;
                    Vector3 interpolatedWorldSpaceFaceNormal = ((v0.worldSpaceNormalOverW*w0 + v1.worldSpaceNormalOverW*w1 + v2.worldSpaceNormalOverW*w2)/invDenom).normalize();
                    double depth = (w0*v0.clip.z/v0.clip.w + w1*v1.clip.z/v1.clip.w + w2*v2.clip.z/v2.clip.w) / (w0/v0.clip.w + w1/v1.clip.w + w2/v2.clip.w);

                    Color shadedColor = renderer.shadingManager.get()->shadeColorFR(
                        renderer.getCamera()->transform.getPosition(),
                        interpolatedWorldSpaceCoords, interpolatedWorldSpaceFaceNormal, baseColor);

                   /*
                    double u_num = w0*u_over_w[0] + w1*u_over_w[k] + w2*u_over_w[k+1];
                    double v_num = w0*v_over_w[0] + w1*v_over_w[k] + w2*v_over_w[k+1];
                    double invDen = denom;
                    double texU = u_num / invDen;
                    double texV = v_num / invDen;
                    */
                //calculating interpolated normals for gouroud shading
                    /*

                    Vector3 nv0 = transformedVertexNormals[obj.faceVertexIndices[face]];
                    Vector3 nv1 = transformedVertexNormals[obj.faceVertexIndices[face+1]];
                    Vector3 nv2 = transformedVertexNormals[obj.faceVertexIndices[face+2]];

                    Vector3 wv0 = worldSpaceVertices[obj.faceVertexIndices[face]];
                    Vector3 wv1 = worldSpaceVertices[obj.faceVertexIndices[face+1]];
                    Vector3 wv2 = worldSpaceVertices[obj.faceVertexIndices[face+2]];
                    */

                    /*
                    double interNormalAlpha = ( (nv1.y-nv2.y)*(px-nv2.x) + (nv2.x-nv1.x)*(py-nv2.y))/
                                              ((nv1.y - nv2.y)*(nv0.x - nv2.x)+(nv2.x-nv1.x)*(nv0.y-nv2.y));
                    double interNormalBeta = ( (nv2.y-nv0.y)*(px-nv2.x) + (nv0.x-nv2.x)*(py-nv2.y))/
                                             ((nv1.y - nv2.y)*(nv0.x - nv2.x)+(nv2.x-nv1.x)*(nv0.y-nv2.y));
                    double interNormalGamma = 1.0 - interNormalAlpha - interNormalBeta;
                    Vector3 interNormal = (nv0 * interNormalAlpha) + (nv1*interNormalBeta) + (nv2 * interNormalGamma);
                    */

                    /*
                    Vector3 interNormal = ((nv0 * w0) + (nv1 * w1) + (nv2 * w2) ).normalize();
                    Vector3 worldVertex = ((wv0 * w0) + (wv1 * w1) + (wv2 * w2));
                    Color shadedColor = renderer.shadingManager.get()->shadeColorFR(renderer.getCamera()->transform.getPosition(),
                    */
                     //                                         worldVertex,interNormal,baseColor);
                    /*
                    QRgb sample;
                    if (obj.texture && !obj.texture->image.isNull()){
                        //qDebug("texture loaded");
                        int iu = std::clamp(int(texU * obj.texture->image.width ()),0,obj.texture->image.width ()-1);
                        int iv = std::clamp(int((1-texV) * obj.texture->image.height()),0,obj.texture->image.height()-1);
                        sample = obj.texture->image.pixel(iu,iv);
                    }else{
                        sample = qRgba(obj.viewportDisplay.color.R,
                                       obj.viewportDisplay.color.G,
                                       obj.viewportDisplay.color.B,
                                       255);
                    }
                    Color pix( qBlue(sample), qGreen(sample), qRed(sample), qAlpha(sample) );
                    */
                    // logic of drawing pixel and z-buffor check is in renderer but
                    // updating id-buffer is here (for whatever reason?)
                    //Color baseColor = obj.viewportDisplay.color;
                   // Vector3 normal = transformedNormals[face/3];
                    //VERIFY IF CAMERA ACTUALLY UPDATES POSITION IN EVERY FRAME
                   // Color shadedColor = renderer.shadingManager.get()->shadeColorFR(renderer.getCamera()->transform.getPosition(),
                                                                               //     Vector3(x,y,depth),normal,baseColor);
                    if (renderer.drawPixel(x, y, depth, /*pix*/ /*obj.viewportDisplay.color*/shadedColor)) {
                        // Tylko fillId (faceId)
                        Renderer::IdBufferElement fillEl;
                        fillEl.objectId     = objId;
                        fillEl.faceId       = int(face/3);
                        (*renderer.idBuffer)[y][x] = fillEl;
                    }
                }
            }
        }

        //pass wireframe - drawing edges of rasterizatied triangle

        Renderer::IdBufferElement edgeEl;
        edgeEl.objectId = objId;
        edgeEl.faceId   = int(face/3);
        size_t N = screenVerticesWithDepth.size();
        for (size_t i = 0; i < N; ++i) {
            Vector3 A = screenVerticesWithDepth[i];
            Vector3 B = screenVerticesWithDepth[(i+1)%N];

            A.z = std::max(0.0, A.z - 1e-4);
            B.z = std::max(0.0, B.z - 1e-4);

            int idxA = (i   < 3 ? obj.faceVertexIndices[face+i  ] : -1);
            int idxB = (i+1 < 3 ? obj.faceVertexIndices[face+i+1] : -1);
            if (i == 2) idxB = obj.faceVertexIndices[face];

            edgeEl.edgeVertices = { idxA, idxB };
            renderer.drawLine3D(A, B, edgeEl, obj.viewportDisplay.wireframeColor);
        }
    }
}

double RasterRenderStrategy::edgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
    return (c.x - a.x) * (b.y - a.y)
    - (c.y - a.y) * (b.x - a.x);
}
