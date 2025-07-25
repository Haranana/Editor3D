#include "Rendering/FrontOnlyRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/Camera.h"
#include "Rendering/RenderingSurface.h"
#include "Rendering/LinePainter.h"
#include "Math/Vectors.h"

#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/Renderer.h"
#include "Scene/RenderableObject3D.h"

void FrontOnlyRenderStrategy::render(RenderableObject3D& object, Renderer& renderer, int objId)
{
    /*
    std::vector<Vector4> clipSpaceVertices;
    clipSpaceVertices.reserve(object.vertices.size());
    std::vector<Vector3> cameraSpaceVertices;
    for (auto& v : object.vertices){
        clipSpaceVertices.push_back(renderer.modelToClip(v, object.transform.getTransMatrix()));
        cameraSpaceVertices.push_back(renderer.worldToCamera(renderer.modelToWorld(v , object.transform.getTransMatrix())));
    }
    for (size_t i = 0; i < object.faceVertexIndices.size(); i += 3)
    {

        Vector3 cameraSpaceVertex1 = cameraSpaceVertices[object.faceVertexIndices[i]];
        Vector3 cameraSpaceVertex2 = cameraSpaceVertices[object.faceVertexIndices[i+1]];
        Vector3 cameraSpaceVertex3 = cameraSpaceVertices[object.faceVertexIndices[i+2]];

        Vector3 e1 = cameraSpaceVertex2 - cameraSpaceVertex1;
        Vector3 e2 = cameraSpaceVertex3 - cameraSpaceVertex1;
        Vector3 normal = e1 * e2;

        Vector3 cameraVector = Vector3(-cameraSpaceVertex1.x, -cameraSpaceVertex1.y, -cameraSpaceVertex1.z);

        double angleCos = cosBeetwenVectors(normal, cameraVector);

        if(angleCos <= 0.0) continue;

        Vector4 clipSpaceVertex1 = clipSpaceVertices[object.faceVertexIndices[i]];
        Vector4 clipSpaceVertex2 = clipSpaceVertices[object.faceVertexIndices[i+1]];
        Vector4 clipSpaceVertex3 = clipSpaceVertices[object.faceVertexIndices[i+2]];

        std::vector<Vector4> clippedVectors = renderer.clippingManager->clipTriangle({clipSpaceVertex1, clipSpaceVertex2, clipSpaceVertex3});

        std::vector<Vector3> normalizedVertices;
        for(const auto& clippedVertex: clippedVectors){
            normalizedVertices.push_back(renderer.clipToNdc(clippedVertex));
        }

        std::vector<Vector2> screenVertices;
        for(const auto& normalizedVertex: normalizedVertices){
            screenVertices.push_back(renderer.ndcToScreen(normalizedVertex));
        }

        std::vector<Vector3> screenDepthVertices;
        for(size_t vertexIt = 0; vertexIt < screenVertices.size(); vertexIt++){
            float normalizedDepth = ((clippedVectors[vertexIt].z/ clippedVectors[vertexIt].w)+1.0)*0.5;
            screenDepthVertices.push_back({screenVertices[vertexIt].x, screenVertices[vertexIt].y,normalizedDepth});
        }

        for(size_t vertexIt = 0; vertexIt < screenDepthVertices.size(); vertexIt++){
            Renderer::IdBufferElement el;
            el.objectId = objId;
            el.faceId = i/3;

            int idxA = (vertexIt < 3 ? object.faceVertexIndices[i+vertexIt] : -1);
            int idxB = ((vertexIt+1)<3 ? object.faceVertexIndices[i+vertexIt+1] : -1);
            el.edgeVertices = { idxA, idxB };

            renderer.drawLine3D(screenDepthVertices[vertexIt], screenDepthVertices[ (vertexIt+1) % screenDepthVertices.size() ],el, object.viewportDisplay.color);
        }
    }
*/
}

double FrontOnlyRenderStrategy::cosBeetwenVectors( Vector3& normalVector ,  Vector3& cameraVector){

    return (
        (normalVector.x * cameraVector.x + normalVector.y * cameraVector.y + normalVector.z * cameraVector.z) /
        ( sqrt(pow(normalVector.x,2) + pow(normalVector.y,2) + pow(normalVector.z,2))
         * sqrt(pow(cameraVector.x,2) + pow(cameraVector.y,2) + pow(cameraVector.z,2)) )
        );

}

