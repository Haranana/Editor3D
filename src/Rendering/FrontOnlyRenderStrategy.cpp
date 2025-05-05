#include "Rendering/FrontOnlyRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/Camera.h"
#include "Rendering/RenderingSurface.h"
#include "Rendering/LinePainter.h"
#include "Math/Vectors.h"

#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/RenderableObject3D.h"

void FrontOnlyRenderStrategy::render(RenderableObject3D& object, Renderer& renderer)
{
    auto camera = renderer.getCamera();
    auto renderingSurface = renderer.getRenderingSurface();
    LinePainter linePainter(renderingSurface->getImg());

    Matrix4 modelMatrix = object.transform.getTransMatrix();
    Matrix4 viewMatrix  = camera->getViewMatrix();
    Matrix4 modelView   = viewMatrix * modelMatrix;

    // obliczanie transformedVertices obiektu
    for(int vertexIt = 0; vertexIt < static_cast<int>(object.vertices.size()); vertexIt++){
        auto vert4 = Vectors::vector3to4(object.vertices[vertexIt]);
        Vector4 transformed4 = modelView * vert4;
        object.transformedVertices[vertexIt] = Vectors::vector4to3(transformed4);
    }

    // rysowanie krawedzi dla kazdej trojki wierzcholkow
    for(int i = 0; i < static_cast<int>(object.faceVertexIndices.size()); i += 3){
        int i1 = object.faceVertexIndices[i];
        int i2 = object.faceVertexIndices[i+1];
        int i3 = object.faceVertexIndices[i+2];

        Vector3 v1 = object.transformedVertices[i1];
        Vector3 v2 = object.transformedVertices[i2];
        Vector3 v3 = object.transformedVertices[i3];

        // Obliczanie normalnej jako cross dwoch krawedzi
        Vector3 e1 = v2 - v1;
        Vector3 e2 = v3 - v1;
        Vector3 normal = e1 * e2;

        //  Wektor od obiektu do kamery (getPosition() jest globalnym polozeniem kamery)
        Vector3 cameraVector = camera->transform.getPosition() - v1;

        double angleCos = cosBeetwenVectors(normal, cameraVector);

        // cos>0 => trojkat jest widoczny dla kamery
        if(angleCos > 0.0)
        {
            // projekcja i rysowanie
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

            p1 = p1 + renderingSurface->getMiddle();
            p2 = p2 + renderingSurface->getMiddle();
            p3 = p3 + renderingSurface->getMiddle();

            linePainter.drawLine(p1, p2);
            linePainter.drawLine(p2, p3);
            linePainter.drawLine(p3, p1);
        }
    }
}

double FrontOnlyRenderStrategy::cosBeetwenVectors(const Vector3& normalVector , const Vector3& cameraVector){

    return (
        (normalVector.x * cameraVector.x + normalVector.y * cameraVector.y + normalVector.z * cameraVector.z) /
        ( sqrt(pow(normalVector.x,2) + pow(normalVector.y,2) + pow(normalVector.z,2))
         * sqrt(pow(cameraVector.x,2) + pow(cameraVector.y,2) + pow(cameraVector.z,2)) )
        );

}
