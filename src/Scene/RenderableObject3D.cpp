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

void RenderableObject3D::setRenderStrategy(std::unique_ptr<RenderStrategy> newStrategy) {
    // std::move przenosi własność przekazanego wskaźnika do członka klasy
    renderStrategy = std::move(newStrategy);
}
