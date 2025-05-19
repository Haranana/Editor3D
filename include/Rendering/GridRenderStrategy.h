#ifndef GRIDRENDERSTRATEGY_H
#define GRIDRENDERSTRATEGY_H

#include "RenderStrategy.h"

class RenderableObject3D;
class Renderer;

class GridRenderStrategy : public RenderStrategy {
public:
    void render(RenderableObject3D& object, Renderer& renderer, int objId) override;
};


#endif // GRIDRENDERSTRATEGY_H
