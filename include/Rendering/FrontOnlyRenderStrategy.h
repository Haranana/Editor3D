#ifndef FRONTONLYRENDERSTRATEGY_H
#define FRONTONLYRENDERSTRATEGY_H

#include "RenderStrategy.h"
#include <vector>
#include <cmath>

class RenderableObject3D;
class Renderer;

//Requirements: at least 3 vertices per face

class FrontOnlyRenderStrategy : public RenderStrategy {
public:
    void render(RenderableObject3D& object, Renderer& renderer) override;
private:
    double cosBeetwenVectors( Vector3& normalVector ,  Vector3& cameraVector);
};


#endif // FRONTONLYRENDERSTRATEGY_H
