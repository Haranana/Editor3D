#ifndef RASTERRENDERSTRATEGY_H
#define RASTERRENDERSTRATEGY_H

#include "RenderStrategy.h"
#include <vector>
#include <cmath>

class RenderableObject3D;
class Renderer;

class RasterRenderStrategy : public RenderStrategy {
public:
    void render(RenderableObject3D& object, Renderer& renderer) override;
    double edgeFunction(const Vector3 &a, const Vector3 &b, const Vector3 &c);
};


#endif // RASTERRENDERSTRATEGY_H
