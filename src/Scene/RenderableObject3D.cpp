#include "Scene/RenderableObject3D.h"
#include "Rendering/BasicRenderStrategy.h"

RenderableObject3D::RenderableObject3D() {
    renderStrategy = std::make_unique<BasicRenderStrategy>();
}

RenderableObject3D::RenderableObject3D(const RenderableObject3D& otherObject) :
    vertices(otherObject.vertices),
    transformedVertices(otherObject.transformedVertices),
    faceVertexIndices(otherObject.faceVertexIndices),
    normals(otherObject.normals),
    textureCoords(otherObject.textureCoords)
{
    this->transform = otherObject.transform;
    renderStrategy = std::make_unique<BasicRenderStrategy>();
}

void RenderableObject3D::setRenderMethod(RenderingMethods newRenderMethod){
    this->renderMethod = newRenderMethod;
}

RenderingMethods RenderableObject3D::getRenderMethod(){
    return renderMethod;
}
