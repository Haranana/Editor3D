#ifndef CAMERA_H
#define CAMERA_H

#include "Scene/Object3D.h"

class Camera : public  Object3D{
public:
    double getFov();
    void setFov(double newFov);
    Matrix4 getViewMatrix();

    static constexpr double nearPlane = 0.1;
    static constexpr double farPlane  = 1000.0;
private:

    //wlasciwie to focal length, wylaca nazwa
    //dziala "im wieksze fov tym slabsza perspektywa"
    //raczej odwrotnie niz zazwyczaj ale dziala
    const int defaultFov = 200.0;
    double fov = defaultFov;



};

#endif // CAMERA_H
