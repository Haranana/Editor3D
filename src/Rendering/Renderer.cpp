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


bool Renderer::drawPixel(int x, int y, double z, const Color& color)
{
    std::shared_ptr<std::vector<std::vector<float>>> zBuffer = getZBuffer();
    if (x < 0 || y < 0 || x >= renderingSurface->getImg()->width() || y >=renderingSurface->getImg()->height())
        return false;

    if (z < (*zBuffer)[y][x]) {
        (*zBuffer)[y][x]   = z;
        pixelPainter->drawPixel(Vector2(x,y),color);
        //renderingSurface->getImg()->setPixelColor(x, y, c.toQColor());

        return true;
    }
    return false;
}

void Renderer::drawLine3D(const Vector3& vec1, const Vector3& vec2, const Color& color){
    int dx  = std::abs(vec2.x - vec1.x);
    int dy  = std::abs(vec2.y - vec1.y);
    int sx  = (vec1.x < vec2.x ? 1 : -1);
    int sy  = (vec1.y < vec2.y ? 1 : -1);
    int err = dx - dy;

    int x = vec1.x, y = vec1.y;
    const int steps = std::max(dx, dy);
    for (int i = 0; i <= steps; ++i)
    {
        double t   = steps == 0 ? 0.0 : double(i) / steps;      // 0..1
        double z   = vec1.z + t * (vec2.z - vec1.z);                      // liniowa interpolacja
        drawPixel(x, y, z, color);                    // <‑‑ TEST Z

        int e2 = 2 * err;
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
