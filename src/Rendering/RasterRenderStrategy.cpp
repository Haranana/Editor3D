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

void RasterRenderStrategy::render(RenderableObject3D& object, Renderer& renderer)
{
    std::vector<Vector4> clipSpaceVertices;
    clipSpaceVertices.reserve(object.vertices.size());
    for (auto& v : object.vertices)
        clipSpaceVertices.push_back(renderer.modelToClip(v, object.transform.getTransMatrix()));

    for (size_t i = 0; i < object.faceVertexIndices.size(); i += 3)
    {
        Vector4 clipSpaceVertex1 = clipSpaceVertices[object.faceVertexIndices[i]];
        Vector4 clipSpaceVertex2 = clipSpaceVertices[object.faceVertexIndices[i+1]];
        Vector4 clipSpaceVertex3 = clipSpaceVertices[object.faceVertexIndices[i+2]];

        if (clipSpaceVertex1.x < -clipSpaceVertex1.w && clipSpaceVertex2.x < -clipSpaceVertex2.w && clipSpaceVertex3.x < -clipSpaceVertex3.w) continue; // left
        if (clipSpaceVertex1.x >  clipSpaceVertex1.w && clipSpaceVertex2.x >  clipSpaceVertex2.w && clipSpaceVertex3.x >  clipSpaceVertex3.w) continue; // right
        if (clipSpaceVertex1.y < -clipSpaceVertex1.w && clipSpaceVertex2.y < -clipSpaceVertex2.w && clipSpaceVertex3.y < -clipSpaceVertex3.w) continue; // down
        if (clipSpaceVertex1.y >  clipSpaceVertex1.w && clipSpaceVertex2.y >  clipSpaceVertex2.w && clipSpaceVertex3.y >  clipSpaceVertex3.w) continue; // up
        if (clipSpaceVertex1.z >  clipSpaceVertex1.w && clipSpaceVertex2.z >  clipSpaceVertex2.w && clipSpaceVertex3.z >  clipSpaceVertex3.w) continue; // near
        if (clipSpaceVertex1.z < -clipSpaceVertex1.w && clipSpaceVertex2.z < -clipSpaceVertex2.w && clipSpaceVertex3.z < -clipSpaceVertex3.w) continue; // far

        Vector3 normalizedVertex1 = renderer.clipToNdc(clipSpaceVertex1);
        Vector3 normalizedVertex2 = renderer.clipToNdc(clipSpaceVertex2);
        Vector3 normalizedVertex3 = renderer.clipToNdc(clipSpaceVertex3);

        Vector2 screenVertex1 = renderer.ndcToScreen(normalizedVertex1);
        Vector2 screenVertex2 = renderer.ndcToScreen(normalizedVertex2);
        Vector2 screenVertex3 = renderer.ndcToScreen(normalizedVertex3);

        Vector3 screenVertexWithZ1(screenVertex1.x, screenVertex1.y, -clipSpaceVertex1.z / clipSpaceVertex1.w);
        Vector3 screenVertexWithZ2(screenVertex2.x, screenVertex2.y, -clipSpaceVertex2.z / clipSpaceVertex2.w);
        Vector3 screenVertexWithZ3(screenVertex3.x, screenVertex3.y, -clipSpaceVertex3.z / clipSpaceVertex3.w);

        double w0_inv = 1.0 / clipSpaceVertex1.w,
            w1_inv = 1.0 / clipSpaceVertex2.w,
            w2_inv = 1.0 / clipSpaceVertex3.w;

        double z0_ndc = normalizedVertex1.z,             // 0…1 (Scratchpixel wariant)
            z1_ndc = normalizedVertex2.z,
            z2_ndc = normalizedVertex3.z;

        double minX = std::min({screenVertexWithZ1.x, screenVertexWithZ2.x, screenVertexWithZ3.x});
        double maxX = std::max({screenVertexWithZ1.x, screenVertexWithZ2.x, screenVertexWithZ3.x});
        double minY = std::min({screenVertexWithZ1.y, screenVertexWithZ2.y, screenVertexWithZ3.y});
        double maxY = std::max({screenVertexWithZ1.y, screenVertexWithZ2.y, screenVertexWithZ3.y});

        minX = (int)std::max(0, (int)std::floor(minX));
        maxX = (int)std::min(renderer.getRenderingSurface()->getImg()->width()-1, (int)std::floor(maxX));
        minY = (int)std::max(0, (int)std::floor(minY));
        maxY = (int)std::min(renderer.getRenderingSurface()->getImg()->height()-1, (int)std::floor(maxY));

        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {

                double curPixelX = x+0.5;
                double curPixelY = y+0.5;

                double w0 = edgeFunction(screenVertexWithZ2, screenVertexWithZ3, Vector3(curPixelX, curPixelY, 0));
                double w1 = edgeFunction(screenVertexWithZ3, screenVertexWithZ1, Vector3(curPixelX, curPixelY, 0));
                double w2 = edgeFunction(screenVertexWithZ1, screenVertexWithZ2, Vector3(curPixelX, curPixelY, 0));

                double area = edgeFunction(screenVertexWithZ1, screenVertexWithZ2, screenVertexWithZ3);
                if (area == 0) continue;
                if (area < 0) {
                    w0 = -w0; w1 = -w1; w2 = -w2;
                    area = -area;
                }

                if (w0 < 0 || w1 < 0 || w2 < 0) continue;

                double zOverW = w0*z0_ndc*w0_inv + w1*z1_ndc*w1_inv + w2*z2_ndc*w2_inv;
                double oneOverW = w0*w0_inv + w1*w1_inv + w2*w2_inv;
                double depth = zOverW / oneOverW;            // 0…1, mniejsze = bliżej

                renderer.drawPixel(x, y, depth, object.viewportDisplay.color);

            }
        }
    }

}

double RasterRenderStrategy::edgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
    return (c.x - a.x) * (b.y - a.y)
    - (c.y - a.y) * (b.x - a.x);
}
