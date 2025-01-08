#ifndef RENDERABLEOBJECT3D_H
#define RENDERABLEOBJECT3D_H

#include "Rendering/RenderingMethods.h"
#include "Rendering/RenderStrategy.h"
#include "Object3D.h"

class BasicRenderStrategy;
class RenderStrategy;

class RenderableObject3D : public  Object3D{
public:

    RenderableObject3D();
    RenderableObject3D(const RenderableObject3D& otherObject);

    std::vector<Vector3> vertices;
    std::vector<Vector3> transformedVertices;
    std::vector<std::pair<int , int>> edges;
    std::vector<std::vector<int>> faces;

    void setRenderMethod(RenderingMethods newRenderMethod);
    RenderingMethods getRenderMethod();
    std::unique_ptr<RenderStrategy> renderStrategy;

protected:
    enum RenderingMethods renderMethod = BASIC;
};

#endif // RENDERABLEOBJECT3D_H
