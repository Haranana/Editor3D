#ifndef CAMERA_H
#define CAMERA_H

#include "Scene/Object3D.h"
#include "Math/Vectors.h"

class Camera : public  Object3D{
public:
    double getFov();
    void setFov(double fov);
    Matrix4 getViewMatrix();
    Matrix4 getProjectionMatrix();
    void setPerspective(double fovY, double aspect, double nearPlane, double farPlane);
    std::vector<Vector3> getFrustum() ;

    double fovY = defaultFovY;
    double aspect = 1.0;
    double nearPlane = defaultNearPlane;
    double farPlane  = defaultFarPlane;
private:
    static constexpr double defaultFovY = 60.0;
    static constexpr double defaultNearPlane = 0.1;
    static constexpr double defaultFarPlane = 1000.0;


     //wlasciwie to nie fov tylko odleglosc kamery od obrazu, todo zmienic nazwe,
    //as for 20/07 not sure if used anywhere potential delete
    double fov = 200.0;

};

#endif // CAMERA_H
