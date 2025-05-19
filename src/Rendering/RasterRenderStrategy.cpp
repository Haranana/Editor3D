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

void RasterRenderStrategy::render(RenderableObject3D& obj, Renderer& renderer)
{
    const int width  = renderer.getRenderingSurface()->getImg()->width();
    const int height = renderer.getRenderingSurface()->getImg()->height();

    std::vector<Vector4> clip;
    clip.reserve(obj.vertices.size());
    for (const auto& v : obj.vertices)
        clip.push_back(renderer.modelToClip(v, obj.transform.getTransMatrix()));


    for (size_t face = 0; face < obj.faceVertexIndices.size(); face += 3)
    {
        Vector4 clipV0 = clip[obj.faceVertexIndices[face    ]];
        Vector4 clipV1 = clip[obj.faceVertexIndices[face + 1]];
        Vector4 clipV2 = clip[obj.faceVertexIndices[face + 2]];

        std::vector<Vector4> clippedPoly =
            renderer.clippingManager->clipTriangle({ clipV0, clipV1, clipV2 });

        if (clippedPoly.size() < 3) continue;

        for (size_t k = 1; k + 1 < clippedPoly.size(); ++k)
        {

            Vector4 cv0 = clippedPoly[0];
            Vector4 cv1 = clippedPoly[k];
            Vector4 cv2 = clippedPoly[k + 1];

            if (cv0.w <= 0 || cv1.w <= 0 || cv2.w <= 0) continue;

            Vector3 n0 = renderer.clipToNdc(cv0),
                n1 = renderer.clipToNdc(cv1),
                n2 = renderer.clipToNdc(cv2);

            Vector2 s0 = renderer.ndcToScreen(n0),
                s1 = renderer.ndcToScreen(n1),
                s2 = renderer.ndcToScreen(n2);

            int minX = std::max(0,        int(std::floor(std::min({s0.x,s1.x,s2.x}))));
            int maxX = std::min(width-1,  int(std::ceil (std::max({s0.x,s1.x,s2.x}))));
            int minY = std::max(0,        int(std::floor(std::min({s0.y,s1.y,s2.y}))));
            int maxY = std::min(height-1, int(std::ceil (std::max({s0.y,s1.y,s2.y}))));

            double area = (s1.x - s0.x)*(s2.y - s0.y) - (s1.y - s0.y)*(s2.x - s0.x);
            if (area == 0) continue;
            double invArea = 1.0 / area;

            double invW0 = 1.0 / cv0.w,
                invW1 = 1.0 / cv1.w,
                invW2 = 1.0 / cv2.w;

            double z0 = n0.z, z1 = n1.z, z2 = n2.z;

            for (int y = minY; y <= maxY; ++y)
                for (int x = minX; x <= maxX; ++x)
                {
                    double px = x + 0.5,  py = y + 0.5;

                    double w0 = ((s1.x - px)*(s2.y - py) - (s1.y - py)*(s2.x - px)) * invArea;
                    double w1 = ((s2.x - px)*(s0.y - py) - (s2.y - py)*(s0.x - px)) * invArea;
                    double w2 = 1.0 - w0 - w1;

                    if (w0 < 0 || w1 < 0 || w2 < 0) continue;

                    double sumInvW = w0*invW0 + w1*invW1 + w2*invW2;
                    double depth   = (w0*z0*invW0 + w1*z1*invW1 + w2*z2*invW2) / sumInvW;

                    renderer.drawPixel(x, y, depth, obj.viewportDisplay.color);
                }
        }
    }
}

double RasterRenderStrategy::edgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
    return (c.x - a.x) * (b.y - a.y)
    - (c.y - a.y) * (b.x - a.x);
}
