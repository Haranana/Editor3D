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
#include "Rendering/HitDetectionManager.h"
#include <QImage>
#include "Rendering/ShadingManager.h"
#include <limits>
#include "Math/Matrices.h"
#include "Scene/DistantLight.h"
#include "Scene/PointLight.h"
#include "Scene/SpotLight.h"
#include "IdBufferElement.h"
#include "PaintTool.h"

class Renderer{
public:

    enum DebugMode{
        DEBUG_SHADOWMAP,
        NONE
    };

    Renderer(
        std::shared_ptr<QImage> img,
        std::shared_ptr<Scene> scene,
        std::shared_ptr<Camera> camera);

    void renderScene();
    void clearRenderingSurface();

    void setRenderingSurface(std::shared_ptr<RenderingSurface> newRenderingSurface);
    void setScene(std::shared_ptr<Scene> newScene);
    void setCamera(std::shared_ptr<Camera> newCamera);
    std::shared_ptr<Camera> getCamera();
    std::shared_ptr<Scene> getScene();
    std::shared_ptr<RenderingSurface> getRenderingSurface();
    std::shared_ptr<Buffer<IdBufferElement>> getIdBuffer();

private:

    void renderSceneObjects();
    void renderObject( RenderableObject3D& obj, int objId);
    void updateShadowMaps();
    void shadowMapDepthPass(DistantLight& lightSource, const Matrix4& lightView, const Matrix4& lightProjection);
    void shadowMapDepthPass(PointLight& lightSource);

    Vector4 modelToClip(const Vector3& v, const Matrix4& modelMatrix);
    Vector4 worldToClip(const Vector3& v);
    Vector3 modelToWorld(const Vector3& v, const Matrix4& modelMatrix);
    Vector3 worldToCamera(const Vector3& v);
    Vector3 clipToNdc(const Vector4& v);
    Vector2 ndcToScreen(const Vector3& v);
    Vector3 getFaceNormal(Vector3 v0, Vector3 v1, Vector3 v2, bool inverse = false);
    Vector3 normalToWorld( Transform3D objTransform, const Vector3& normal);

    void clearBuffers();
    void updateCommonMatrices();

    constexpr static DebugMode debugMode = NONE;

    std::shared_ptr<RenderingSurface> renderingSurface;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Buffer<double>>zBuffer;
    std::shared_ptr<Buffer<IdBufferElement>> idBuffer;
    std::shared_ptr<PixelPainter> pixelPainter;
    std::shared_ptr<LinePainter> linePainter;
    std::shared_ptr<ClippingManager> clippingManager;
    std::shared_ptr<ShadingManager> shadingManager;

    PaintTool paintTool;
    Matrix4 viewProjectionMatrix;
    Matrix4 viewMatrix;
    Matrix4 ProjectionMatrix;

};

#endif // RENDERER_H
