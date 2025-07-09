#ifndef BASICRENDERSTRATEGY_H
#define BASICRENDERSTRATEGY_H

#include "RenderStrategy.h"

class RenderableObject3D;
class Renderer;
class Vector3;
class Vector4;

/*
 * Renders every edge of object and nothing more
 *
*/
class BasicRenderStrategy : public RenderStrategy {
public:
    void render(RenderableObject3D& object, Renderer& renderer, int objId) override;
};

#endif // BASICRENDERSTRATEGY_H
