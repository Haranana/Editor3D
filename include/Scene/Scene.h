#ifndef SCENE_H
#define SCENE_H

#include <QImage>
#include <vector>
#include "Object3D.h"
#include <memory>
#include "Light.h"

class Scene{
public:
    void addObject(std::shared_ptr<Object3D> obj);
    void removeObject(int id);
    void removeObject(std::shared_ptr<Object3D> objectPtr);
    void changeObjectHierarchy(int objectId, int targetId);
    int objectsAmount();
    int getSceneObjectId(int uiId);

    std::shared_ptr<Object3D> getObject(int id);

    Vector3 ambientColor {0.2, 0.2, 0.2};
    double  ambientIntensity = 0.12;
    bool    ambientPBR = true;             // weaker diffuse on metallic surfaces

    std::vector<std::shared_ptr<Light>>lightSources;
private:

    std::vector<std::shared_ptr<Object3D>>objectsHierarchy;

};

#endif // SCENE_H
