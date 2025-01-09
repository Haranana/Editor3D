#include "Rendering/Renderer.h"

Renderer::Renderer(
    std::shared_ptr<RenderingSurface> renderingSurface,
    std::shared_ptr<Scene> scene,
    std::shared_ptr<Camera> camera) :
    renderingSurface(renderingSurface) ,
    scene(scene),
    camera(camera) {}

void Renderer::renderScene(){
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


void Renderer::renderSceneObjects(){
    LinePainter linePainter = LinePainter(renderingSurface->getImg());

    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){

        std::shared_ptr<Object3D> object = scene->getObject(objIt);
        if(RenderableObject3D* curObject = dynamic_cast<RenderableObject3D*>(object.get())){
            if(curObject->renderStrategy) curObject->renderStrategy->render(*curObject , *this);

        }
    }
}
