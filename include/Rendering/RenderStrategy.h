#ifndef RENDERSTRATEGY_H
#define RENDERSTRATEGY_H

#include <memory>
#include <Math/Vectors.h>
#include <Math/Vector4.h>
#include <Math/Vector3.h>



class RenderableObject3D;
class Renderer;

class RenderStrategy {
public:
    virtual ~RenderStrategy() = default;
    virtual void render(RenderableObject3D& obj, Renderer& renderer) = 0;
};

#endif // RENDERSTRATEGY_H
