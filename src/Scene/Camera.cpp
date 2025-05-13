#include "Scene/Camera.h"
#include <cmath>

double Camera::getFov(){
    return fov;
}

void Camera::setFov(double newFov){
    fov = newFov;
}

Matrix4 Camera::getViewMatrix()
{
    return transform.getTransMatrix().getInversion();
}

void Camera::setPerspective(double fovY, double aspect, double nearPlane, double farPlane){
    this->fovY = fovY;
    this->aspect = aspect;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}

Matrix4 Camera::getProjectionMatrix(){
    //double rad = fovY * M_PI/180.0;
    //double t = std::tan(rad * 0.5);
    //double nf = 1.0 / (nearPlane - farPlane);

    double scalingFactor = 1/std::tan((fovY/2) * (M_PI/180.0));
    double ascpectScalingFactor = 1/(std::tan((fovY/2) * (M_PI/180.0))* aspect);
    double nearPlaneClipFactor = -((farPlane+nearPlane)/(farPlane - nearPlane));
    double farPlaneClipFactor = -((2* farPlane * nearPlane)/(farPlane - nearPlane));
    return Matrix4({{
        {ascpectScalingFactor, 0,       0,                        0},
        {0,              scalingFactor,   0,                      0},
        {0,              0, nearPlaneClipFactor, farPlaneClipFactor},
        {0,              0,      -1,                              0}
    }});
}
