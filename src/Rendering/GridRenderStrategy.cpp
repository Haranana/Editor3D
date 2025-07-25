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

void GridRenderStrategy::render(RenderableObject3D& object, Renderer& renderer, int objId)
{
    bool even = true;


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

        float normalizedDepth1 = ((clipSpaceVertex1.z/ clipSpaceVertex1.w)+1.0)*0.5;
        float normalizedDepth2 = ((clipSpaceVertex1.z/ clipSpaceVertex1.w)+1.0)*0.5;
        float normalizedDepth3 = ((clipSpaceVertex1.z/ clipSpaceVertex1.w)+1.0)*0.5;

        Vector3 screenVertexWithZ1(screenVertex1.x, screenVertex1.y, normalizedDepth1);
        Vector3 screenVertexWithZ2(screenVertex2.x, screenVertex2.y, normalizedDepth2);
        Vector3 screenVertexWithZ3(screenVertex3.x, screenVertex3.y, normalizedDepth3);

        Renderer::IdBufferElement element1,element2,element3;
        element1.mock = true;
        element2.mock = true;
        element3.mock = true;
        renderer.drawLine3D(screenVertexWithZ1, screenVertexWithZ2,element1, object.viewportDisplay.color);
        if(!even) renderer.drawLine3D(screenVertexWithZ2, screenVertexWithZ3,element2, object.viewportDisplay.color);
        if(even) renderer.drawLine3D(screenVertexWithZ3, screenVertexWithZ1,element3, object.viewportDisplay.color);
        even = even? false : true;

        /*
        Renderer::IdBufferElement element1;
        element1.faceId = object.faceVertexIndices[i];
        element1.objectId = objId;
        element1.faceId = i/3;
        element1.edgeVertices = {object.faceVertexIndices[i], object.faceVertexIndices[i+1]};
        renderer.drawLine3D(screenVertexWithZ1, screenVertexWithZ2,element1, object.viewportDisplay.color);

        Renderer::IdBufferElement element2;
        element1.faceId = object.faceVertexIndices[i];
        element1.objectId = objId;
        element1.faceId = i/3;
        element1.edgeVertices = {object.faceVertexIndices[i], object.faceVertexIndices[i+1]};
        if(!even) renderer.drawLine3D(screenVertexWithZ2, screenVertexWithZ3, object.viewportDisplay.color);


        Renderer::IdBufferElement element3;
        if(even) renderer.drawLine3D(screenVertexWithZ3, screenVertexWithZ1, object.viewportDisplay.color);
        even = even? false : true;
        */
    }
}
