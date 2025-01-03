#ifndef SCENE_H
#define SCENE_H

#include <QImage>
#include <vector>
#include "Object3D.h"
#include <memory>

class Scene{
public:

    void addObject(std::shared_ptr<Object3D> obj);
    void removeObject(int id);
    void removeObject(std::shared_ptr<Object3D> objectPtr);
    void changeObjectHierarchy(int objectId, int targetId);
    int objectsAmount();

    std::shared_ptr<Object3D> getObject(int id);
private:
    std::vector<std::shared_ptr<Object3D>>objectsHierarchy;
};

#endif // SCENE_H
