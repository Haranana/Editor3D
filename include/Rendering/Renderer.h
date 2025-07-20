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

/* Draws Scene Objects on Rendering Surface according to object Hierarchy
 * Since the program is in very primitve stages and Objects consist only of lines, all logic will be happening here
 * the output of Renderer is then shown to the user
 * in the future, Renderer will also be probably used to rendering some UI elements such as selection arrows or grids
 */

/* As for 09/07:
 * - Przechowuje id oraz z buffery
 * - przechowuje renderingSurface, scene i kamere
 * - przechowuje narzedzia do rysowania
 * - renderuje scene: wywoluje render() na kazdym aplikowalnym obiekcie
 * - przenoszeni vectory miedzy przestrzeniami
*/
class Renderer{
public:

    struct IdBufferElement{
        bool isEmpty = true;
        bool mock = false;
        int objectId = -1;
        int faceId = -1;
        int vertexId = -1;
        std::pair<int, int> edgeVertices = {-1,-1};
    };

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

    Vector3 getFaceNormal(Vector3 v0, Vector3 v1, Vector3 v2, bool inverse = false);
    Vector3 faceNormalToWorld( Transform3D objTransform, const Vector3& normal);

    //do poprawienia na perspective correct (1/z) Chyba juz jest idk
    //probably should create different class, some Painter3D for those but eh
    void drawLine3D(const Vector3& vec1,  const Vector3& vec2, IdBufferElement& idBufferElement, const Color& color = Color(255,255,255,255));
    void drawCircle3D(const Vector3& v, IdBufferElement& idBufferElement,int radius, const Color& color = Color(255,255,255,255));
    void drawLine3D(const Vector3& vec1,  const Vector3& vec2, const Color& color = Color(255,255,255,255));
    void drawCircle3D(const Vector3& v,int radius, const Color& color = Color(255,255,255,255));
    void drawSquare3D(const Vector3& v,int radius, const Color& color = Color(255,255,255,255));
    //Probuje pokolorowac dany pixel z uwzglednieniem zBuffora, zwraca informacje czy pixel zostal pokolorowany
    bool drawPixel(int x, int y, double z, const Color& c);

    std::shared_ptr<Buffer<IdBufferElement>> idBuffer; //pewnie powinno byc prywatne
    std::shared_ptr<Buffer<double>>zBuffer; //rowniez pewnie powinno byc prywatne
    std::shared_ptr<Camera> getCamera();

    std::shared_ptr<Scene> getScene();
    std::shared_ptr<RenderingSurface> getRenderingSurface();
    //std::shared_ptr<std::vector<std::vector<float>>>getZBuffer();

    std::shared_ptr<PixelPainter> pixelPainter;
    std::shared_ptr<LinePainter> linePainter;

    std::shared_ptr<ClippingManager> clippingManager;
    std::shared_ptr<ShadingManager> shadingManager;
    //std::shared_ptr<HitDetectionManager> hitDetectionManager;
    //std::shared_ptr<std::vector<std::vector<IdBufferElement>>>idBuffer;

private:

    //reset every value inside zBuffer to infinity - moved to separate classes
    //void resetZBuffer();
    //void resetIdBuffer();

    void renderObject( RenderableObject3D& obj, int objId);

    Matrix4 viewProjectionMatrix;
    Matrix4 viewMatrix;
    Matrix4 ProjectionMatrix;

    void renderSceneObjects();
    void highlightObjectsSelectedElements();

    //can't find any instance where it's used, probably safe to delete
    const int zOffsetDefaultValue = -200;
    int zOffset = zOffsetDefaultValue;

    std::shared_ptr<RenderingSurface> renderingSurface;
    std::shared_ptr<Scene> scene;
    std::shared_ptr<Camera> camera;
    //std::shared_ptr<std::vector<std::vector<float>>>zBuffer;



};

#endif // RENDERER_H
