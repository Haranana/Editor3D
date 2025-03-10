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
    auto camera = renderer.getCamera();
    auto renderingSurface = renderer.getRenderingSurface();
    LinePainter linePainter(renderingSurface->getImg());
    PixelPainter pixelPainter(renderingSurface->getImg());

    //transformacja punktow
    for(int vertexIt = 0; vertexIt < static_cast<int>(object.vertices.size()); vertexIt++){
        auto vert4 = Vectors::vector3to4(object.vertices[vertexIt]);
        auto transformed4 = object.transform.getTransMatrix() * vert4;
        object.transformedVertices[vertexIt] = Vectors::vector4to3(transformed4);
    }

    //projekcja perspektywiczna
    std::vector<Vector3>screenCoordinates;
    for(const auto& vertex : object.transformedVertices){


        Vector2 screenPoint(
            (vertex.x * camera->getFov()) / (camera->getFov() + vertex.z),
            (vertex.y * camera->getFov()) / (camera->getFov() + vertex.z)
            );

        screenCoordinates.push_back(Vector3(screenPoint.x + renderingSurface->getMiddle().x,
                                            screenPoint.y + renderingSurface->getMiddle().y,
                                            vertex.z));

    }

    for (int it = 0; it < static_cast<int>(object.faceVertexIndices.size()); it += 3) {

        Vector3 v1 = screenCoordinates[object.faceVertexIndices[it]];
        Vector3 v2 = screenCoordinates[object.faceVertexIndices[it+1]];
        Vector3 v3 = screenCoordinates[object.faceVertexIndices[it+2]];

        double minX = std::min({v1.x, v2.x, v3.x});
        double maxX = std::max({v1.x, v2.x, v3.x});
        double minY = std::min({v1.y, v2.y, v3.y});
        double maxY = std::max({v1.y, v2.y, v3.y});

        minX = (int)std::max(0, (int)std::floor(minX));
        maxX = (int)std::min(renderer.getRenderingSurface()->getImg()->width()-1, (int)std::floor(maxX));
        minY = (int)std::max(0, (int)std::floor(minY));
        maxY = (int)std::min(renderer.getRenderingSurface()->getImg()->height()-1, (int)std::floor(maxY));

        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {

                double curPixelX = x+0.5;
                double curPixelY = y+0.5;

                double w0 = edgeFunction(v2, v3, Vector3(curPixelX, curPixelY, 0));
                double w1 = edgeFunction(v3, v1, Vector3(curPixelX, curPixelY, 0));
                double w2 = edgeFunction(v1, v2, Vector3(curPixelX, curPixelY, 0));

                // Aby mieć 'obszar' trójkąta, najpierw policzmy area = edgeFunction(v1,v2,v3)
                double area = edgeFunction(v1, v2, v3);

                // W zależności od znaku area, warunek w0>=0,... lub <=0
                // Zakładam, że area może być >0, więc wewnątrz jest w0>=0 itd.
                // (Jeśli odwrotne winding, trzeba by sprawdzić <0)
                if (area < 0.0) {
                    w0 = -w0; w1 = -w1; w2 = -w2; area = -area;
                }

                if (w0 >= 0 && w1 >= 0 && w2 >= 0){

                    // 5c) interpolacja Z - (prosta: z w wierzchołkach)
                    //    (na razie "liniowo" – docelowo: perspective-correct = 1/z)
                    double zInterpol =
                        (v1.z * w0 + v2.z * w1 + v3.z * w2) / area;

                    if (zInterpol < (*renderer.getZBuffer())[y][x])
                    {
                        // Nowy fragment jest bliżej
                        (*renderer.getZBuffer())[y][x] = zInterpol;

                        //pixelPainter.drawPixel(Vector2(x , y) , Color(0,0,200,255));
                        pixelPainter.drawPixel(Vector2(x , y) , object.viewportDisplay.color);
                    }
                }
            }
        }
    }
}//sad

double RasterRenderStrategy::edgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
    return (c.x - a.x) * (b.y - a.y)
    - (c.y - a.y) * (b.x - a.x);
}
