#ifndef CAMERA_H
#define CAMERA_H

#include "Scene/Object3D.h"

class Camera : public  Object3D{
public:
    double getFov();
    void setFov(double fov);
    Matrix4 getViewMatrix();
    Matrix4 getProjectionMatrix();
    void setPerspective(double fovY, double aspect, double nearPlane, double farPlane);

    double fovY = defaultFovY;
    double aspect = 1.0;
    double nearPlane = 0.1;
    double farPlane  = 1000.0;
private:
    static constexpr double defaultFovY = 60.0;
    double fov = 200.0; //wlasciwie to nie fov tylko odleglosc kamery od obrazu, todo zmienic nazwe

};

#endif // CAMERA_H
