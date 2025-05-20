#include "Rendering/RasterRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/Camera.h"
#include "Rendering/RenderingSurface.h"
#include "Rendering/LinePainter.h"
#include "Rendering/PixelPainter.h"
#include "Math/Vectors.h"

#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/RenderableObject3D.h"

void RasterRenderStrategy::render(RenderableObject3D& obj,
                                  Renderer& renderer,
                                  int objId)
{
    const int width  = renderer.getRenderingSurface()->getImg()->width();
    const int height = renderer.getRenderingSurface()->getImg()->height();

    // 1. Zbuduj clip-space dla wszystkich wierzchołków
    std::vector<Vector4> clip;
    clip.reserve(obj.vertices.size());
    Matrix4 M = obj.transform.getTransMatrix();
    for (auto& v : obj.vertices)
        clip.push_back(renderer.modelToClip(v, M));

    // 2. Dla każdej ściany (face)
    for (size_t face = 0; face < obj.faceVertexIndices.size(); face += 3)
    {
        // a) Pobierz oryginalne wierzchołki w clip-space
        Vector4 cv0 = clip[obj.faceVertexIndices[face    ]];
        Vector4 cv1 = clip[obj.faceVertexIndices[face + 1]];
        Vector4 cv2 = clip[obj.faceVertexIndices[face + 2]];

        // b) Klipuj
        auto clippedPoly =
            renderer.clippingManager->clipTriangle({cv0, cv1, cv2});
        if (clippedPoly.size() < 3) continue;

        // c) Rzut do NDC → ekran i liczenie depth
        std::vector<Vector3> normalizedVertices;
        normalizedVertices.reserve(clippedPoly.size());
        for (auto& cv : clippedPoly)
            normalizedVertices.push_back(renderer.clipToNdc(cv));

        std::vector<Vector2> screenVertices;
        screenVertices.reserve(normalizedVertices.size());
        for (auto& nv : normalizedVertices)
            screenVertices.push_back(renderer.ndcToScreen(nv));

        std::vector<Vector3> screenDepthVertices;
        screenDepthVertices.reserve(clippedPoly.size());
        for (size_t i = 0; i < clippedPoly.size(); ++i) {
            auto& sv = screenVertices[i];
            auto& cv = clippedPoly[i];
            double depth = -cv.z / cv.w;
            screenDepthVertices.push_back({ sv.x, sv.y, depth });
        }

        // d) Fill-pass: Twoja oryginalna pętla barycentryczna
        for (size_t k = 1; k + 1 < clippedPoly.size(); ++k)
        {
            auto& A = screenVertices[0];
            auto& B = screenVertices[k];
            auto& C = screenVertices[k+1];

            // bounding box
            int minX = std::max(0, int(std::floor(std::min({A.x,B.x,C.x}))));
            int maxX = std::min(width-1, int(std::ceil(std::max({A.x,B.x,C.x}))));
            int minY = std::max(0, int(std::floor(std::min({A.y,B.y,C.y}))));
            int maxY = std::min(height-1, int(std::ceil(std::max({A.y,B.y,C.y}))));

            double area = (B.x - A.x)*(C.y - A.y)
                          - (B.y - A.y)*(C.x - A.x);
            if (area == 0.0) continue;
            double invArea = 1.0 / area;

            // invW
            double invW0 = 1.0 / clippedPoly[0].w;
            double invW1 = 1.0 / clippedPoly[k].w;
            double invW2 = 1.0 / clippedPoly[k+1].w;

            // raster fill
            for (int y = minY; y <= maxY; ++y) {
                for (int x = minX; x <= maxX; ++x) {
                    double px = x + 0.5, py = y + 0.5;
                    double w0 = ((B.x-px)*(C.y-py) - (B.y-py)*(C.x-px)) * invArea;
                    double w1 = ((C.x-px)*(A.y-py) - (C.y-py)*(A.x-px)) * invArea;
                    double w2 = 1.0 - w0 - w1;
                    if (w0<0 || w1<0 || w2<0) continue;

                    double z0 = screenDepthVertices[0].z;
                    double z1 = screenDepthVertices[k].z;
                    double z2 = screenDepthVertices[k+1].z;
                    double sumInvW = w0*invW0 + w1*invW1 + w2*invW2;
                    double depth = (w0*z0*invW0
                                    + w1*z1*invW1
                                    + w2*z2*invW2)
                                   / sumInvW;

                    if (renderer.drawPixel(x, y, depth, obj.viewportDisplay.color)) {
                        // Tylko fillId (faceId)
                        HitDetectionManager::IdBufferElement fillEl;
                        fillEl.objectId     = objId;
                        fillEl.faceId       = int(face/3);
                        (*renderer.hitDetectionManager->idBuffer)[y][x] = fillEl;
                    }
                }
            }
        }
        Color edgeColor = Colors::Orange;
        HitDetectionManager::IdBufferElement edgeEl;
        edgeEl.objectId = objId;
        edgeEl.faceId   = int(face/3);
        size_t N = screenDepthVertices.size();      // ← po klipowaniu!
        for (size_t i = 0; i < N; ++i) {
            Vector3 A = screenDepthVertices[i];
            Vector3 B = screenDepthVertices[(i+1)%N];

            // 1-px papierowy offset głębi
            A.z -= 1e-4;
            B.z -= 1e-4;

            // oryginalne indeksy tylko dla pierwszych 3 punktów
            int idxA = (i   < 3 ? obj.faceVertexIndices[face+i  ] : -1);
            int idxB = (i+1 < 3 ? obj.faceVertexIndices[face+i+1] : -1);
            if (i == 2) idxB = obj.faceVertexIndices[face];      // wrap 2→0

            edgeEl.edgeVertices = { idxA, idxB };
            renderer.drawLine3D(A, B, edgeEl, obj.viewportDisplay.wireframeColor);
        }
    }
}

double RasterRenderStrategy::edgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
    return (c.x - a.x) * (b.y - a.y)
    - (c.y - a.y) * (b.x - a.x);
}
