#include "Rendering/Renderer.h"

Renderer::Renderer(
    std::shared_ptr<RenderingSurface> renderingSurface,
    std::shared_ptr<Scene> scene,
    std::shared_ptr<Camera> camera) :
    renderingSurface(renderingSurface) ,
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
}

void Renderer::renderScene(){
    resetZBuffer();
    renderSceneObjects();
}

void Renderer::setRenderingSurface(std::shared_ptr<RenderingSurface> newRenderingSurface){
    renderingSurface = newRenderingSurface;
}

void Renderer::setScene(std::shared_ptr<Scene> newScene){
    scene = newScene;
}

void Renderer::setCamera(std::shared_ptr<Camera> newCamera){
    camera = newCamera;
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
        std::cout<<object->transform.getPosition()<<std::endl;;
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
