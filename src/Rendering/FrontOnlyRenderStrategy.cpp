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
    std::shared_ptr<Camera> camera = renderer.getCamera();
    std::shared_ptr<RenderingSurface> renderingSurface = renderer.getRenderingSurface();

    LinePainter linePainter(renderingSurface->getImg());

    for(int vertexIt = 0; vertexIt < static_cast<int>(object.vertices.size()); vertexIt++){
        object.transformedVertices[vertexIt] = Vectors::vector4to3(object.transform.getTransMatrix()
                                                                   * Vectors::vector3to4(object.vertices[vertexIt]));
    }

    std::vector<std::pair<int , int>> edgesToRender;

    for(const auto& curFace : object.faces){

        Vector3 normalVector = getNormalVector(object , curFace);

        double angleCos = cosBeetwenCameraAndNormal(Vector3(camera->transform.getPosition() - object.transformedVertices[curFace[0]]),
                                                           normalVector);
        if(angleCos>0){
            edgesToRender.push_back( std::make_pair(curFace[curFace.size()-1]   ,curFace[0] ));
            for(int vertexIt = 1 ; vertexIt <static_cast<int>(curFace.size()) ; vertexIt++ ){
                edgesToRender.push_back(std::make_pair(curFace[vertexIt-1]   ,curFace[vertexIt] ));
            }
        }

    }

    for(const auto& curEdge : edgesToRender){
        Vector3 firstVertex = object.transformedVertices[curEdge.first];
        Vector3 secondVertex = object.transformedVertices[curEdge.second];

        Vector2 firstPointPerspective = Vector2( (firstVertex.x*camera->getFov()) / (camera->getFov() + firstVertex.z) ,
                                                (firstVertex.y*camera->getFov()) / (camera->getFov() + firstVertex.z));

        Vector2 secondPointPerspective = Vector2( (secondVertex.x*camera->getFov()) / (camera->getFov() + secondVertex.z) ,
                                                 (secondVertex.y*camera->getFov()) / (camera->getFov() + secondVertex.z));

        // Currently creating objects at middle of the rendering Surface, in future should use object3D starting Position parameter
        linePainter.drawLine(Vector2(firstPointPerspective.x , firstPointPerspective.y) + renderingSurface->getMiddle() ,
                             Vector2(secondPointPerspective.x , secondPointPerspective.y) + renderingSurface->getMiddle());
    }
}

Vector3 FrontOnlyRenderStrategy::getNormalVector(const RenderableObject3D& object , const std::vector<int>& objectFace){
    Vector3 v1 = object.transformedVertices[objectFace[1]] - object.transformedVertices[objectFace[0]];
    Vector3 v2 = object.transformedVertices[objectFace[objectFace.size()-1]] - object.transformedVertices[objectFace[0]];

    return v1*v2;
}

double FrontOnlyRenderStrategy::cosBeetwenCameraAndNormal(const Vector3& normalVector , const Vector3& cameraVector){

    return (
        (normalVector.x * cameraVector.x + normalVector.y * cameraVector.y + normalVector.z * cameraVector.z) /
        ( sqrt(pow(normalVector.x,2) + pow(normalVector.y,2) + pow(normalVector.z,2))
         * sqrt(pow(cameraVector.x,2) + pow(cameraVector.y,2) + pow(cameraVector.z,2)) )
        );

}
