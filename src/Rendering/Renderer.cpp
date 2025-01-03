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

void Renderer::renderSceneObjects(){

    /* In this early version of program where the only rendered object is cube and there are no other render methods
     * whole logic is inside renderSceneObjects, in the future it will most likely be moved to dedicated module
     */
    LinePainter linePainter = LinePainter(renderingSurface->getImg());
    for(int objIt = scene->objectsAmount()-1 ; objIt >= 0 ; objIt-- ){

        std::shared_ptr<Object3D> curObject = scene->getObject(objIt);

        for(const auto& it : curObject->edges){
            Vector3 firstVertex = it.first;
            Vector3 secondVertex = it.second;

            firstVertex = Vectors::vector4to3(curObject->transform.getTransMatrix() * Vectors::vector3to4(firstVertex));
            secondVertex = Vectors::vector4to3(curObject->transform.getTransMatrix() * Vectors::vector3to4(secondVertex));

            Vector2 firstPointPerspective = Vector2( (firstVertex.x*camera->getFov()) / (camera->getFov() + firstVertex.z) ,
                                                (firstVertex.y*camera->getFov()) / (camera->getFov() + firstVertex.z));

            Vector2 secondPointPerspective = Vector2( (secondVertex.x*camera->getFov()) / (camera->getFov() + secondVertex.z) ,
                                                 (secondVertex.y*camera->getFov()) / (camera->getFov() + secondVertex.z));

            //std::cout<<"Drawing line for Points : "<< firstPointPerspective << "  -  "<<secondPointPerspective<<" fov and z: "<<fov <<","<< firstPoint.z<<" , "<<fov<<","<< secondPoint.z<<std::endl;

            /*
             * Currently creating objects at middle of the rendering Surface, in future should use object3D starting Position parameter
             */
            linePainter.drawLine(Vector2(firstPointPerspective.x , firstPointPerspective.y) + renderingSurface->getMiddle() ,
                                  Vector2(secondPointPerspective.x , secondPointPerspective.y) + renderingSurface->getMiddle());

        }
    }
}
