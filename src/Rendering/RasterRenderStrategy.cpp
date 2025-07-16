#include "Rendering/RasterRenderStrategy.h"
#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/RenderableObject3D.h"

void RasterRenderStrategy::render(RenderableObject3D& obj,
                                  Renderer& renderer,
                                  int objId)
{
    //preparation
    const int width  = renderer.getRenderingSurface()->getImg()->width();
    const int height = renderer.getRenderingSurface()->getImg()->height();

    //building clip-space for all vertices
    std::vector<Vector4> clip;
    std::vector<Vector2> uv;
    Matrix4 M = obj.transform.getTransMatrix();
    for (auto& v : obj.vertices){
        clip.push_back(renderer.modelToClip(v, M));
        uv.push_back(obj.textureCoords[ &v - &obj.vertices[0] ]);
    }

    std::vector<Vector3> transformedNormals;
    for(auto& n : obj.normals){
        transformedNormals.push_back(renderer.faceNormalToWorld(obj.transform, n));
    }

    //iterate through every face
    for (size_t face = 0; face < obj.faceVertexIndices.size(); face += 3)
    {
        //parameters for simple shading
        Color baseColor = obj.viewportDisplay.color;
        Vector3 normal = transformedNormals[face/3];
        Vector3 faceCenterWorldSpace = (renderer.modelToWorld(obj.vertices[obj.faceVertexIndices[face]] , obj.transform.getTransMatrix()) +
                                        renderer.modelToWorld(obj.vertices[obj.faceVertexIndices[face+1]] , obj.transform.getTransMatrix()) +
                                        renderer.modelToWorld(obj.vertices[obj.faceVertexIndices[face+2]] , obj.transform.getTransMatrix()))*(1.0/3.0);

        Color shadedColor = renderer.shadingManager.get()->shadeColorFR(renderer.getCamera()->transform.getPosition(),
                                                                        faceCenterWorldSpace,normal,baseColor);


        //original vertices in clip-space
        Vector4 cv0 = clip[obj.faceVertexIndices[face    ]];
        Vector4 cv1 = clip[obj.faceVertexIndices[face + 1]];
        Vector4 cv2 = clip[obj.faceVertexIndices[face + 2]];

        //clipping triangle
        auto clippedPoly =
            renderer.clippingManager->clipTriangle({cv0, cv1, cv2});
        if (clippedPoly.size() < 3) continue;

        // clip -> ndc
        std::vector<Vector3> normalizedVertices;
        normalizedVertices.reserve(clippedPoly.size());
        for (auto& cv : clippedPoly)
            normalizedVertices.push_back(renderer.clipToNdc(cv));


        // ndc -> screen
        std::vector<Vector2> screenVertices;
        screenVertices.reserve(normalizedVertices.size());
        for (auto& nv : normalizedVertices)
            screenVertices.push_back(renderer.ndcToScreen(nv));


        // preparing data
        // screenDepthVertices = {screen.x , screen.y, normalized.z}; normalization in range [0,1]
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



        // d) Fill-pass: Twoja oryginalna pętla barycentryczna
        for (size_t k = 1; k + 1 < clippedPoly.size(); ++k)
        {
            // Fan-triangulation
            auto& A = screenVertices[0];
            auto& B = screenVertices[k];
            auto& C = screenVertices[k+1];

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

            for (int y = minY; y <= maxY; ++y) {
                for (int x = minX; x <= maxX; ++x) {

                    double px = x + 0.5, py = y + 0.5; //middle of current pixel

                    //checking if pixel is withing triangle using baricentric coordinates
                    double w0 = ((B.x-px)*(C.y-py) - (B.y-py)*(C.x-px)) * invArea;
                    double w1 = ((C.x-px)*(A.y-py) - (C.y-py)*(A.x-px)) * invArea;
                    double w2 = 1.0 - w0 - w1;
                    if (w0<0 || w1<0 || w2<0) continue;

                    //depth interpolation / perspective-correct interpolation
                    double num   = w0 * clipZ[0] * invW[0]
                                 + w1 * clipZ[k] * invW[k]
                                 + w2 * clipZ[k+1] * invW[k+1];

                    double denom = w0 * invW[0]
                                   + w1 * invW[k]
                                   + w2 * invW[k+1];

                    double u_num = w0*u_over_w[0] + w1*u_over_w[k] + w2*u_over_w[k+1];
                    double v_num = w0*v_over_w[0] + w1*v_over_w[k] + w2*v_over_w[k+1];
                    double invDen = denom;
                    double texU = u_num / invDen;
                    double texV = v_num / invDen;

                    double depth = num/denom;
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
        size_t N = screenDepthVertices.size();
        for (size_t i = 0; i < N; ++i) {
            Vector3 A = screenDepthVertices[i];
            Vector3 B = screenDepthVertices[(i+1)%N];

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
