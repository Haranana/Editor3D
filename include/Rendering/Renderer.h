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
#include "Rendering/LightingManager.h"
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

    struct FlatShadingFaceData{
        std::vector<Vector3> pointToLightDirection;
        std::vector<Vector3> combinedLight;
        std::vector<Vector3> diffuseNoAlbedo;
        std::vector<Vector3> specularWithLight;
        std::vector<double>  NdotL;
        Vector3 flatFaceNormal;
        Vector3 flatWorldCentroid;
    };


    void renderSceneObjects();
    void renderObject( RenderableObject3D& obj, int objId);
    GouraudShadingFaceData collectGouraudPerFaceData(Triangle<ClippingManager::ClippedVertex> fanTriangleClipped,
                                                     const Triangle3& fanTriangleRawWorldSpace,
                                                     const RenderableObject3D& obj);

    FlatShadingFaceData collectFlatPerFaceData(const Triangle3& fanTriangleRawWorldSpace,
                                               const RenderableObject3D& obj);

    void updateShadowMaps();
    void shadowMapDepthPass(DistantLight& lightSource);
    void shadowMapDepthPass(PointLight& lightSource);
    void shadowMapDepthPass(SpotLight& spotLight);
    bool shouldCullBackFace(const Triangle3& face);


    double getShadowAmount(const Light& light, const Vector3& worldSpacePoint, const Vector3& normal, const Triangle3& worldSpaceFan);

    Vector3 getPointToLight(const Vector3& worldSpacePoint, const Light& light);

    double getVisibility(const double shadowAmount);

    Vector3 getShadowTintModifier(const Vector3& albedo, const Vector3& combinedLight,  double shadowIntensity, double shadowAmount);

    Vector3 getCombinedLight(const Vector3& color, double intensity, double attenuation);

    Vector3 interpolateCombinedLight(const Triangle3& combinedLightOverW, const Triangle<double>& baricentricFactor, double invDenom);

    Vector3 getDiffuseModifier(const RenderableObject3D& obj, const Vector3& combinedLight, const Vector3& pointWorldCoords,
                              const Vector3& faceNormal, const Vector3& pointToLightDir, const Vector3& albedo, double visibility = 1.0, bool brdf = false);

    //If diffuse it supposed to be brdf, then vertices in diffuseNoAlbedoInVerticesOverW should already be with brdf modifier
    //also assumes that diffuse already is multiplied by combinedLight
    Vector3 interpolateDiffuseModifier(const Triangle3& diffuseNoAlbedoInVerticesOverW , const Triangle<double>& baricentricFactor,
                                       const Vector3& albedo, double invDenom, double visibility);

    Vector3 getSpecularModifier(const RenderableObject3D& obj, const Vector3& combinedLight, const Vector3& pointWorldCoords,
                               const Vector3& faceNormal, const Vector3& pointToLightDir, double visibility = 1.0);

    //assumes that vertices in triangle are already modified by combinedLight
    Vector3 interpolatSpecularModifier(const Triangle3& specularInVerticesOverW , const Triangle<double>& baricentricFactor,
                                       double invDenom, double visibility);



    struct Bias{
        double biasAddition{};
        Vector3 pointForDepthCheck{};
        PointLight::ShadowMapFace face;
    };

    double calculateBias(const Light& light, const Vector3& worldSpacePoint, const Vector3& normal, const Triangle3& fanWorldSpace, int pcfKernelSize = 0);
    double calculateShadowAmount(const Buffer<double>& shadowMap ,const Light& light, const Vector2& point,
                                 double receiverDepth, double bias);
    double shadowMapDepthValueFor(const Light& light,const Vector3& pointWorldSpace, int face = 0);

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
    std::shared_ptr<ClippingManager> clippingManager;
    std::shared_ptr<LightingManager> lightingManager;


    PaintTool paintTool;
    Matrix4 viewProjectionMatrix;
    Matrix4 viewMatrix;
    Matrix4 ProjectionMatrix;

};

#endif // RENDERER_H
