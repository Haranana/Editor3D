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

        renderer.drawLine3D(screenVertexWithZ1, screenVertexWithZ2, object.viewportDisplay.color);
        renderer.drawLine3D(screenVertexWithZ2, screenVertexWithZ3, object.viewportDisplay.color);
        renderer.drawLine3D(screenVertexWithZ3, screenVertexWithZ1, object.viewportDisplay.color);
    }
}

double FrontOnlyRenderStrategy::cosBeetwenVectors( Vector3& normalVector ,  Vector3& cameraVector){

    return (
        (normalVector.x * cameraVector.x + normalVector.y * cameraVector.y + normalVector.z * cameraVector.z) /
        ( sqrt(pow(normalVector.x,2) + pow(normalVector.y,2) + pow(normalVector.z,2))
         * sqrt(pow(cameraVector.x,2) + pow(cameraVector.y,2) + pow(cameraVector.z,2)) )
        );

}

