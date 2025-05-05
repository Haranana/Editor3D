#ifndef CAMERA_H
#define CAMERA_H

#include "Math/Transform3D.h"
#include "Scene/Object3D.h"

class Camera : public  Object3D{
public:

    Transform3D transform;

    double getFov();
    void setFov(double newFov);
private:
    const int defaultFov = 200.0;
    double fov = defaultFov;
};

#endif // CAMERA_H
