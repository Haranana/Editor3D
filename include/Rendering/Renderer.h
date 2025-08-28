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
#include "Math/Triangle.h"
#include "Math/Triangle3.h"
#include "Math/Utility.h"
#include "Math/NoiseManager.h"
#include <QString>
#include "Rendering/FilteringManager.h"

class Renderer{
public:

    enum DebugMode{
        DEBUG_SHADOWMAP,
        NONE
    };

    struct RenderStatistics{
        //count of every object for which at least 1 pixel was drawn on scene
        int objects = 0;

        //3 for each drawn face
        int vertices = 0;

        //count of each traingulated triangle which drawn at least 1 pixel
        int faces = 0;

        //for each edge where at least 1 pixel was drawn
        int edges = 0;

        void clear(){
            objects = 0;
            vertices = 0;
            faces = 0;
            edges = 0;
        }

        QString toQString(){
            return QString("obj: %1, vert: %2, faces: %3, edges: %4").arg(objects).arg(vertices).arg(faces).arg(edges);
        }
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
    RenderStatistics stats;

private:

    struct GouraudShadingVertexData{
        std::vector<Vector3> combinedLightOverW{};
        std::vector<Vector3> diffuseNoAlbedoOverW{};
        std::vector<Vector3> specularOverW{};

        Vector3 normal{};
    };

    struct GouraudShadingFaceData{
        Triangle<GouraudShadingVertexData> face;
        Vector3 faceNormal;
        Vector3 centroid;
    };

    void renderSceneObjects();
    void renderObject( RenderableObject3D& obj, int objId);
    GouraudShadingFaceData collectGouraudPerFaceData(Triangle<ClippingManager::ClippedVertex> fanTriangleClipped,
                                                     const Triangle3& fanTriangleRawWorldSpace,
                                                     const RenderableObject3D& obj);
    void updateShadowMaps();
    void shadowMapDepthPass(DistantLight& lightSource);
    void shadowMapDepthPass(PointLight& lightSource);
    void shadowMapDepthPass(SpotLight& spotLight);
    bool shouldCullBackFace(const Triangle3& face);

    struct Bias{
        double biasAddition{};
        Vector3 pointForDepthCheck{};
        PointLight::ShadowMapFace face;
    };

    void calculateBias(const std::shared_ptr<Light>& light, const Vector3& point, const Vector3& normal,
                       Vector3& pointForDepthCheck, double& biasAddition, Triangle3& fanWorldCoords,
                        PointLight::ShadowMapFace* outFace = nullptr, int pcfKernelSize = 0);


    Bias calculateBias(const std::shared_ptr<Light>& light, const Vector3& point, const Vector3& normal,
                       Triangle3& fanWorldCoords, PointLight::ShadowMapFace face = PointLight::ShadowMapFace::NEGATIVE_X, int pcfKernelSize = 0);

    double calculateShadowAmount(const Buffer<double>& shadowMap, const Vector2& point,
                                 double receiverDepth, double bias, const Light& light);

    Vector4 modelToClip(const Vector3& v, const Matrix4& modelMatrix);
    Vector4 worldToClip(const Vector3& v);
    Vector3 modelToWorld(const Vector3& v, const Matrix4& modelMatrix);
    Vector3 worldToCamera(const Vector3& v);
    Vector3 clipToNdc(const Vector4& v);
    Vector2 ndcToScreen(const Vector3& v);
    Vector3 getFaceNormal(Vector3 v0, Vector3 v1, Vector3 v2, bool inverse = false);
    Vector3 normalToWorld( Transform3D objTransform, const Vector3& normal);
    std::pair<int,int> ndcToShadowMapTexel(const Vector2& ndc, const Buffer<double>&shadowMap);

    void clearBuffers();
    void updateCommonMatrices();

    constexpr static DebugMode debugMode = NONE;
    constexpr static double doubleBias = 1e-4;

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
