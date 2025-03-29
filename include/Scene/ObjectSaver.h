#ifndef OBJECTSAVER_H
#define OBJECTSAVER_H

#include "include/Scene/RenderableObject3D.h"


class ObjectSaver{
public:
    void saveObject(std::shared_ptr<RenderableObject3D>object);
    void saveScene(); //TODO
private:

    std::string generateFileName();
};

#endif // OBJECTSAVER_H
