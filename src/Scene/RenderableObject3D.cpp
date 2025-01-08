#include "Scene/RenderableObject3D.h"
#include "Rendering/BasicRenderStrategy.h"

RenderableObject3D::RenderableObject3D() {
    renderStrategy = std::make_unique<BasicRenderStrategy>();
}

RenderableObject3D::RenderableObject3D(const RenderableObject3D& otherObject) :
    vertices(otherObject.vertices),
    edges(otherObject.edges),
    faces(otherObject.faces)
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
