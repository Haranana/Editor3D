#include "Rendering/Renderer.h"

Renderer::Renderer(
    //std::shared_ptr<RenderingSurface> renderingSurface,
    std::shared_ptr<QImage> img,
    std::shared_ptr<Scene> scene,
    std::shared_ptr<Camera> camera) :
    renderingSurface(std::make_shared<RenderingSurface>(img)) ,
    scene(scene),
    camera(camera)
{
    zBuffer = std::make_shared<Buffer<float>>(img->width(), img->height(), std::numeric_limits<float>::infinity());
    idBuffer = std::make_shared<Buffer<IdBufferElement>>(img->width(), img->height(), IdBufferElement{});

    pixelPainter = std::make_shared<PixelPainter>(renderingSurface->getImg());
    linePainter = std::make_shared<LinePainter>(renderingSurface->getImg());
    clippingManager = std::make_shared<ClippingManager>();
}

void Renderer::renderScene(){
    idBuffer->clear();
    zBuffer->clear();

    viewMatrix = camera->getViewMatrix();
    ProjectionMatrix = camera->getProjectionMatrix();
    viewProjectionMatrix = ProjectionMatrix * viewMatrix;

    renderSceneObjects();
}

Vector4 Renderer::modelToClip(const Vector3& v, const Matrix4& modelMatrix){
    Vector4 result = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix * Vector4(v.x,v.y,v.z, 1.0);
    return result;
}

Vector4 Renderer::worldToClip(const Vector3& v){
    Vector4 result = camera->getProjectionMatrix() * camera->getViewMatrix() * Vector4(v.x,v.y,v.z, 1.0);
    return result;
}

Vector3 Renderer::modelToWorld(const Vector3& v, const Matrix4& modelMatrix){
    Vector4 resultHom = modelMatrix * Vector4(v.x,v.y,v.z, 1.0);
    Vector3 result = Vector3(resultHom.x, resultHom.y, resultHom.z);
    return result;
}

Vector3 Renderer::worldToCamera(const Vector3& v){
    Vector4 resultHom = camera->getViewMatrix() * Vector4(v.x,v.y,v.z, 1.0);
    Vector3 result = Vector3(resultHom.x, resultHom.y, resultHom.z);
    return result;
}

Vector3 Renderer::clipToNdc(const Vector4& v){
    return Vector3(v.x/v.w, v.y/v.w, v.z/v.w);
}

Vector2 Renderer::ndcToScreen(const Vector3& v){
    return Vector2(
        (v.x + 1) * 0.5 * renderingSurface->getImg()->width(),
        (1 - (v.y + 1)*0.5) * renderingSurface->getImg()->height());
}

void Renderer::setRenderingSurface(std::shared_ptr<RenderingSurface> newRenderingSurface){
    renderingSurface = newRenderingSurface;
}

void Renderer::clearRenderingSurface(){
    pixelPainter->fillImage(Colors::Black);
}

void Renderer::setScene(std::shared_ptr<Scene> newScene){
    scene = newScene;
}

void Renderer::setCamera(std::shared_ptr<Camera> newCamera){
    camera = newCamera;
}

bool Renderer::drawPixel(int x, int y, double depth, const Color& color)
{
    //std::shared_ptr<std::vector<std::vector<float>>> zBuffer = getZBuffer();
    if (x < 0 || y < 0 || x >= renderingSurface->getImg()->width() || y >=renderingSurface->getImg()->height())
        return false;

    if (depth < (*zBuffer)[y][x]) {
        (*zBuffer)[y][x]   = depth;
        pixelPainter->drawPixel(Vector2(x,y),color);

        return true;
    }
    return false;
}

void Renderer::drawLine3D(const Vector3& aScr, const Vector3& bScr,IdBufferElement& idBufferElement, const Color& color)
{

    IdBufferElement element = idBufferElement;
    bool isMock = element.mock;
    // aScr, bScr: x,y w pikselach, z = depth 0..1 (mniejsza = bliżej)
    int x0 = int(std::round(aScr.x));
    int y0 = int(std::round(aScr.y));
    int x1 = int(std::round(bScr.x));
    int y1 = int(std::round(bScr.y));

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    int steps = std::max(dx, dy);

    for (int i = 0, x = x0, y = y0; i <= steps; ++i)
    {
        double t = steps ? double(i) / steps : 0.0;
        double depth = aScr.z + t * (bScr.z - aScr.z);
        if( drawPixel(x, y, depth, color)){
            if(!isMock){
            if(i == 0 && element.edgeVertices.first != -1){
                element.vertexId = element.edgeVertices.first;
            }else if(i == steps && element.edgeVertices.second != -1){
                element.vertexId = element.edgeVertices.second;
            }else{
                element.vertexId = -1;
            }
            element.isEmpty = false;
            (*idBuffer)[y][x] = element;
            }
        }

        int e2 = err << 1;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 <  dx) { err += dx; y += sy; }
    }
}

std::shared_ptr<Camera> Renderer::getCamera(){
    return camera;
}

std::shared_ptr<Scene> Renderer::getScene(){
    return scene;
}

std::shared_ptr<RenderingSurface> Renderer::getRenderingSurface(){
    return renderingSurface;
}

/*
std::shared_ptr<std::vector<std::vector<float>>> Renderer::getZBuffer(){
    return zBuffer;
}
*/

void Renderer::renderSceneObjects(){
    LinePainter linePainter = LinePainter(renderingSurface->getImg());

    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){

        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){
            if(curObject->renderStrategy) curObject->renderStrategy->render(*curObject , *this, objIt);
        }
    }
}

/*
void Renderer::resetZBuffer(){
    for(int itY = 0; itY < static_cast<int>(zBuffer->size()) ; itY++){
        for(int itX = 0; itX < static_cast<int>((*zBuffer)[itY].size()) ; itX++){
            (*zBuffer)[itY][itX] = std::numeric_limits<float>::infinity();
        }
    }
}
*/
