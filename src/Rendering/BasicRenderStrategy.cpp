#include "Rendering/BasicRenderStrategy.h"
#include "Scene/Object3D.h"
#include "Rendering/Renderer.h"
#include "Scene/Camera.h"             // if needed
#include "Rendering/RenderingSurface.h"// if needed
#include "Rendering/LinePainter.h"     // if needed
#include "Math/Vectors.h"             // if needed

#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/RenderableObject3D.h"

void BasicRenderStrategy::render(RenderableObject3D& object, Renderer& renderer)
{
    auto camera = renderer.getCamera();
    auto renderingSurface = renderer.getRenderingSurface();

    LinePainter linePainter(renderingSurface->getImg());

    for(int vertexIt = 0; vertexIt < static_cast<int>(object.vertices.size()); vertexIt++){
        object.transformedVertices[vertexIt] = Vectors::vector4to3(object.transform.getTransMatrix()
                                                                       * Vectors::vector3to4(object.vertices[vertexIt]));
    }

    for(const auto& curEdge : object.edges){
        Vector3 firstVertex = object.transformedVertices[curEdge.first];
        Vector3 secondVertex = object.transformedVertices[curEdge.second];

        Vector2 firstPointPerspective = Vector2( (firstVertex.x*camera->getFov()) / (camera->getFov() + firstVertex.z) ,
                                                (firstVertex.y*camera->getFov()) / (camera->getFov() + firstVertex.z));

        Vector2 secondPointPerspective = Vector2( (secondVertex.x*camera->getFov()) / (camera->getFov() + secondVertex.z) ,
                                                 (secondVertex.y*camera->getFov()) / (camera->getFov() + secondVertex.z));

        // Currently creating objects at middle of the rendering Surface, in future should use object3D starting Position parameter

        // Debug cout
        //  std::cout<<"Drawing line for Points : "<< firstPointPerspective << "  -  "<<secondPointPerspective<<" fov and z: "<<camera->getFov() <<","<< firstVertex.z<<" , "<<camera->getFov()<<","<< secondVertex.z<<std::endl;
        linePainter.drawLine(Vector2(firstPointPerspective.x , firstPointPerspective.y) + renderingSurface->getMiddle() ,
                             Vector2(secondPointPerspective.x , secondPointPerspective.y) + renderingSurface->getMiddle());
    }
}

