#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "RenderingSurface.h"
#include "Scene/Scene.h"
#include "Scene/Camera.h"
#include "Scene/Object3D.h"
#include "Math/Vectors.h"
#include "LinePainter.h"
#include "RenderStrategy.h"
#include "Scene/RenderableObject3D.h"

/* Draws Scene Objects on Rendering Surface according to object Hierarchy
 * Since the program is in very primitve stages and Objects consist only of lines, all logic will be happening here
 * the output of Renderer is then shown to the user
 * in the future, Renderer will also be probably used to rendering some UI elements such as selection arrows or grids
 */
class Renderer{
public:

    Renderer(
        std::shared_ptr<RenderingSurface> renderingSurface,
        std::shared_ptr<Scene> scene,
        std::shared_ptr<Camera> camera);

    void renderScene();

    void setRenderingSurface(std::shared_ptr<RenderingSurface> newRenderingSurface);
    void setScene(std::shared_ptr<Scene> newScene);
    void setCamera(std::shared_ptr<Camera> newCamera);

    std::shared_ptr<Camera> getCamera();
    std::shared_ptr<Scene> getScene();
    std::shared_ptr<RenderingSurface> getRenderingSurface();

private:

    void renderSceneObjects();

    const int zOffsetDefaultValue = -200;
    int zOffset = zOffsetDefaultValue;

    std::shared_ptr<RenderingSurface> renderingSurface;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;

};

#endif // RENDERER_H
