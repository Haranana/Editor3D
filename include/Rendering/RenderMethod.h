#ifndef RENDERMETHOD_H
#define RENDERMETHOD_H

#include <memory>

class Object3D;
class Camera;
class RenderingSurface;

typedef void (*RenderMethod)(std::shared_ptr<Object3D> object,
                             std::shared_ptr<Camera> camera,
                             std::shared_ptr<RenderingSurface> renderingSurface);

#endif // RENDERMETHOD_H
