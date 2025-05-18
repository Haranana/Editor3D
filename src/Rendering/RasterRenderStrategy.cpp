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
    const int width = renderer.getRenderingSurface()->getImg()->width();
    const int height = renderer.getRenderingSurface()->getImg()->height();


    std::vector<Vector4> clip;
    for (size_t it = 0; it < obj.vertices.size(); it++)
        clip.push_back(renderer.modelToClip(obj.vertices[it], obj.transform.getTransMatrix()));

    for (size_t face = 0; face < obj.faceVertexIndices.size(); face += 3)
    {
        int vertexIndex0 = obj.faceVertexIndices[face];
        int vertexIndex1 = obj.faceVertexIndices[face+1];
        int vertexIndex2 = obj.faceVertexIndices[face+2];

        Vector4 clippedVertex0 = clip[vertexIndex0];
        Vector4 clippedVertex1 = clip[vertexIndex1];
        Vector4 clippedVertex2 = clip[vertexIndex2];

        if (clippedVertex0.w <= 0 || clippedVertex1.w <= 0 || clippedVertex2.w <= 0) continue;
        if ( std::abs(clippedVertex0.x) > clippedVertex0.w && std::abs(clippedVertex1.x) > clippedVertex1.w && std::abs(clippedVertex2.x) > clippedVertex2.w) continue;
        if ( std::abs(clippedVertex0.y) > clippedVertex0.w && std::abs(clippedVertex1.y) > clippedVertex1.w && std::abs(clippedVertex2.y) > clippedVertex2.w) continue;
        if ( clippedVertex0.z >  clippedVertex0.w &&  clippedVertex1.z >  clippedVertex1.w &&  clippedVertex2.z >  clippedVertex2.w) continue;   // przed near
        if (-clippedVertex0.z > clippedVertex0.w && -clippedVertex1.z > clippedVertex1.w && -clippedVertex2.z > clippedVertex2.w) continue;     // za far

        Vector3 n0 = renderer.clipToNdc(clippedVertex0), n1 = renderer.clipToNdc(clippedVertex1), n2 = renderer.clipToNdc(clippedVertex2);
        Vector2 s0 = renderer.ndcToScreen(n0), s1 = renderer.ndcToScreen(n1), s2 = renderer.ndcToScreen(n2);

        int minX = std::max(0,   int(std::floor(std::min({s0.x,s1.x,s2.x}))));
        int maxX = std::min(width-1, int(std::ceil (std::max({s0.x,s1.x,s2.x}))));
        int minY = std::max(0,   int(std::floor(std::min({s0.y,s1.y,s2.y}))));
        int maxY = std::min(height-1, int(std::ceil (std::max({s0.y,s1.y,s2.y}))));

        double area = (s1.x - s0.x)*(s2.y - s0.y) - (s1.y - s0.y)*(s2.x - s0.x);
        if (area == 0) continue;
        double invArea = 1.0 / area;


        double invW0 = 1.0 / clippedVertex0.w,
            invW1 = 1.0 / clippedVertex1.w,
            invW2 = 1.0 / clippedVertex2.w;

        double z0 = n0.z, z1 = n1.z, z2 = n2.z; // już 0-1 (OpenGL style)

        for (int y = minY; y <= maxY; ++y)
            for (int x = minX; x <= maxX; ++x)
            {
                double px = x + 0.5, py = y + 0.5;

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

double RasterRenderStrategy::edgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
    return (c.x - a.x) * (b.y - a.y)
    - (c.y - a.y) * (b.x - a.x);
}
