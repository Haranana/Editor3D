#ifndef BASICRENDERSTRATEGY_H
#define BASICRENDERSTRATEGY_H

#include "RenderStrategy.h"

class RenderableObject3D;
class Renderer;

class BasicRenderStrategy : public RenderStrategy {
public:
    void render(RenderableObject3D& object, Renderer& renderer) override;
};

#endif // BASICRENDERSTRATEGY_H
