#ifndef CAMERA_H
#define CAMERA_H

#include "Scene/Object3D.h"

class Camera : public  Object3D{
public:
    double getFov();
    void setFov(double newFov);
    Matrix4 getViewMatrix();
private:
    const int defaultFov = 200.0;
    double fov = defaultFov;
};

#endif // CAMERA_H
