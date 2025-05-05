#ifndef SCENE_H
#define SCENE_H

#include <QImage>
#include <vector>
#include "Object3D.h"
#include "Camera.h"
#include <memory>

class Scene{
private:
    struct SpecialSceneObjects{
        std::shared_ptr<Camera> defaultCamera = nullptr;
    };

public:
    void addObject(std::shared_ptr<Object3D> obj);
    void removeObject(int id);
    void removeObject(std::shared_ptr<Object3D> objectPtr);
    void changeObjectHierarchy(int objectId, int targetId);
    int objectsAmount();
    int getSceneObjectId(int uiId);

    std::shared_ptr<Object3D> getObject(int id);

    SpecialSceneObjects specialSceneObjects;

private:

    std::vector<std::shared_ptr<Object3D>>objectsHierarchy;


};

#endif // SCENE_H
