#include "Scene/Scene.h"

void Scene::addObject(std::shared_ptr<Object3D> obj){
    objectsHierarchy.push_back(obj);
}

void Scene::removeObject(int id){
    objectsHierarchy.erase(objectsHierarchy.begin() + id);
}

void Scene::removeObject(std::shared_ptr<Object3D> objectPtr){
    objectsHierarchy.erase(find(objectsHierarchy.begin() , objectsHierarchy.end() , objectPtr));
}

void Scene::changeObjectHierarchy(int objectId, int targetId){
    std::shared_ptr<Object3D> movedObj = objectsHierarchy[objectId];
    objectsHierarchy.erase(objectsHierarchy.begin() + objectId);
    objectsHierarchy.insert( objectsHierarchy.begin() + targetId , movedObj);
}

int Scene::objectsAmount(){
    return objectsHierarchy.size();
}

std::shared_ptr<Object3D> Scene::getObject(int id){
    if(id>=(int)objectsHierarchy.size()) return nullptr;
    return objectsHierarchy[id];
}
