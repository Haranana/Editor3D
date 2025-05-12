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
    zBuffer = std::make_shared<std::vector<std::vector<float>>>();
    for(int itY = 0; itY < static_cast<int>(renderingSurface->getImg()->height()) ; itY++){
        zBuffer->push_back({});
        for(int itX = 0; itX < static_cast<int>(renderingSurface->getImg()->width()) ; itX++){
            (*zBuffer)[itY].push_back(std::numeric_limits<float>::infinity());
        }
    }

    pixelPainter = std::make_shared<PixelPainter>(renderingSurface->getImg());
    linePainter = std::make_shared<LinePainter>(renderingSurface->getImg());
}

void Renderer::renderScene(){
    resetZBuffer();
    renderSceneObjects();
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

Vector2 Renderer::projectToScreen(const Vector3& cameraVector){
    double invW = 1.0 / (camera->getFov() + cameraVector.z);   // zawsze >0 po klipie
    return {
        cameraVector.x * camera->getFov() * invW + renderingSurface->getMiddle().x,
        cameraVector.y * camera->getFov() * invW + renderingSurface->getMiddle().y
    };
}

bool Renderer::insideNearPlane(const Vector3& v){
    return (-v.z) >= camera->nearPlane;
}

Vector3 Renderer::clipLineBehindNearPlane(const Vector3& v1, const Vector3& v2){
    double intersectionPoint = (camera->nearPlane - (-v1.z) ) / ((-v2.z) - (-v1.z));
    Vector3 iPoint = v1 + (v2 - v1) * intersectionPoint;
    return iPoint;
}



bool Renderer::drawPixel(int x, int y, double depth, const Color& color)
{
    std::shared_ptr<std::vector<std::vector<float>>> zBuffer = getZBuffer();
    if (x < 0 || y < 0 || x >= renderingSurface->getImg()->width() || y >=renderingSurface->getImg()->height())
        return false;

    if (depth < (*zBuffer)[y][x]) {
        (*zBuffer)[y][x]   = depth;
        pixelPainter->drawPixel(Vector2(x,y),color);
        //renderingSurface->getImg()->setPixelColor(x, y, c.toQColor());

        return true;
    }
    return false;
}

void Renderer::drawLine3D(const Vector3& cameraVectorA, const Vector3& cameraVectorB,
                          const Color& color)
{
    /* --- clipping --- */
    bool inA = insideNearPlane(cameraVectorA);
    bool inB = insideNearPlane(cameraVectorB);
    if (!inA && !inB) return;

    Vector3 v1 =cameraVectorA;
    Vector3 v2 =cameraVectorB;
    if (!inA) v1 = clipLineBehindNearPlane(cameraVectorA, cameraVectorB);
    if (!inB) v2 = clipLineBehindNearPlane(cameraVectorB, cameraVectorA);

    /* --- projekcja do pikseli --- */
    Vector2 a2D = projectToScreen(v1);
    Vector2 b2D = projectToScreen(v2);

    int x0 = int(std::round(a2D.x));
    int y0 = int(std::round(a2D.y));
    int x1 = int(std::round(b2D.x));
    int y1 = int(std::round(b2D.y));

    /* --- klasyczny Bresenham + interpolacja depth --- */
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    int steps = std::max(dx, dy);

    for (int i = 0, x = x0, y = y0; i <= steps; ++i)
    {
        double t = steps ? double(i) / steps : 0.0;
        double depth = -(v1.z + t * (v2.z - v1.z));   // <-- zamiast cameraVectorA/B
        drawPixel(x, y, depth, color);

        int e2 = err * 2;
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

std::shared_ptr<std::vector<std::vector<float>>> Renderer::getZBuffer(){
    return zBuffer;
}


void Renderer::renderSceneObjects(){
    LinePainter linePainter = LinePainter(renderingSurface->getImg());

    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){

        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){
            if(curObject->renderStrategy) curObject->renderStrategy->render(*curObject , *this);
        }
    }
}

void Renderer::resetZBuffer(){
    for(int itY = 0; itY < static_cast<int>(zBuffer->size()) ; itY++){
        for(int itX = 0; itX < static_cast<int>((*zBuffer)[itY].size()) ; itX++){
            (*zBuffer)[itY][itX] = std::numeric_limits<float>::infinity();
        }
    }
}
