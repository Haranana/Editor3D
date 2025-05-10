#include "Rendering/GridRenderStrategy.h"
#include "Scene/Object3D.h"
#include "Rendering/Renderer.h"
#include "Scene/Camera.h"
#include "Rendering/RenderingSurface.h"
#include "Rendering/LinePainter.h"
#include "Math/Vectors.h"
#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/RenderableObject3D.h"

void GridRenderStrategy::render(RenderableObject3D& object, Renderer& renderer)
{
    std::shared_ptr<Camera> camera = renderer.getCamera();
    std::shared_ptr<RenderingSurface> renderingSurface = renderer.getRenderingSurface();

    LinePainter linePainter(renderingSurface->getImg());

    Matrix4 modelMatrix = object.transform.getTransMatrix();
    Matrix4 viewMatrix  = camera->getViewMatrix();
    Matrix4 modelView   = viewMatrix * modelMatrix;

    // obliczanie transformedVertices obiektu
    for(int vertexIt = 0; vertexIt < static_cast<int>(object.vertices.size()); vertexIt++){
        // Mnożenie przez macierz transformacji (pozycja, rotacja, skalowanie)
        auto vert4 = Vectors::vector3to4(object.vertices[vertexIt]);
        Vector4 transformed4 = modelView * vert4;
        object.transformedVertices[vertexIt] = Vectors::vector4to3(transformed4);
    }

    bool even = true;
    // rysowanie krawedzi dla kazdej trojki wierzcholkow
    for(int i = 0; i < static_cast<int>(object.faceVertexIndices.size()); i += 3){

        int i1 = object.faceVertexIndices[i];
        int i2 = object.faceVertexIndices[i+1];
        int i3 = object.faceVertexIndices[i+2];

        Vector3 v1 = object.transformedVertices[i1];
        Vector3 v2 = object.transformedVertices[i2];
        Vector3 v3 = object.transformedVertices[i3];

        // projekcja perspektywiczna
        Vector2 p1(
            (v1.x * camera->getFov()) / (camera->getFov() + v1.z),
            (v1.y * camera->getFov()) / (camera->getFov() + v1.z)
            );
        Vector2 p2(
            (v2.x * camera->getFov()) / (camera->getFov() + v2.z),
            (v2.y * camera->getFov()) / (camera->getFov() + v2.z)
            );

        Vector2 p3(
            (v3.x * camera->getFov()) / (camera->getFov() + v3.z),
            (v3.y * camera->getFov()) / (camera->getFov() + v3.z)
            );

        // przesuniecie do środka ekranu
        p1 = p1 + renderingSurface->getMiddle();
        p2 = p2 + renderingSurface->getMiddle();
        p3 = p3 + renderingSurface->getMiddle();

        // rysowanie trojkata
        linePainter.drawLine(p1, p2);
        if(!even)linePainter.drawLine(p2, p3);
        if(even)linePainter.drawLine(p3, p1);

        even = even? false : true;
    }
}
