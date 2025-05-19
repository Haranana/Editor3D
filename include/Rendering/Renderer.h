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
#include "Rendering/PixelPainter.h"
#include "Rendering/ClippingManager.h"
#include "Rendering/LinePainter.h"
#include <QImage>

/* Draws Scene Objects on Rendering Surface according to object Hierarchy
 * Since the program is in very primitve stages and Objects consist only of lines, all logic will be happening here
 * the output of Renderer is then shown to the user
 * in the future, Renderer will also be probably used to rendering some UI elements such as selection arrows or grids
 */
class Renderer{
public:

    Renderer(
        //std::shared_ptr<RenderingSurface> renderingSurface,
        std::shared_ptr<QImage> img,
        std::shared_ptr<Scene> scene,
        std::shared_ptr<Camera> camera);

    void renderScene();

    void setRenderingSurface(std::shared_ptr<RenderingSurface> newRenderingSurface);
    void clearRenderingSurface();
    void setScene(std::shared_ptr<Scene> newScene);
    void setCamera(std::shared_ptr<Camera> newCamera);

    Vector4 modelToClip(const Vector3& v, const Matrix4& modelMatrix);
    Vector4 worldToClip(const Vector3& v);
    Vector3 modelToWorld(const Vector3& v, const Matrix4& modelMatrix);
    Vector3 worldToCamera(const Vector3& v);
    Vector3 clipToNdc(const Vector4& v);
    Vector2 ndcToScreen(const Vector3& v);

    //Probuje pokolorowac dany pixel z uwzglednieniem zBuffora, zwraca informacje czy pixel zostal pokolorowany
    bool drawPixel(int x, int y, double z, const Color& c);

    //do poprawienia na perspective correct (1/z)
    void drawLine3D(const Vector3& vec1,  const Vector3& vec2, const Color& color = Color(255,255,255,255));

    std::shared_ptr<Camera> getCamera();
    std::shared_ptr<Scene> getScene();
    std::shared_ptr<RenderingSurface> getRenderingSurface();
    std::shared_ptr<std::vector<std::vector<float>>>getZBuffer();

    std::shared_ptr<PixelPainter> pixelPainter;
    std::shared_ptr<LinePainter> linePainter;

    std::shared_ptr<ClippingManager> clippingManager;


private:

    //reset every value inside zBuffer to infinity
    void resetZBuffer();

    Matrix4 viewProjectionMatrix;
    Matrix4 viewMatrix;
    Matrix4 ProjectionMatrix;

    void renderSceneObjects();

    const int zOffsetDefaultValue = -200;
    int zOffset = zOffsetDefaultValue;

    std::shared_ptr<RenderingSurface> renderingSurface;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<std::vector<std::vector<float>>>zBuffer;


};

#endif // RENDERER_H
