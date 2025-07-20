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

std::vector<Vector3> Camera::getFrustum() {

    std::vector<Vector3> frustum(8);
    Vector3 pos = this->transform.getPosition();

    Vector3 forward = Vectors::vector4to3(this->transform.getTransMatrix().getCol(2)*(-1.0)).normalize();
    Vector3 up = Vectors::vector4to3(this->transform.getTransMatrix().getCol(1)*(-1.0)).normalize();
    Vector3 right = Vectors::vector4to3(this->transform.getTransMatrix().getCol(0)*(-1.0)).normalize();

    double fovYRad = fovY * M_PI/180.0;
    double tanFovY = tan(fovYRad * 0.5);

    double nearHeight = 2*tanFovY*nearPlane;
    double nearWidth = nearHeight * aspect;
    double farHeight = 2*tanFovY*farPlane;
    double farWidth = farHeight*aspect;

    Vector3 nearCenter = pos + forward * nearPlane;
    Vector3 farCenter = pos + forward * farPlane;

    frustum[0] = nearCenter + up * (nearHeight/2) - right * (nearWidth/2); //ntl
    frustum[1] = nearCenter + up * (nearHeight/2) + right * (nearWidth/2); //ntr
    frustum[2] = nearCenter - up * (nearHeight/2) - right * (nearWidth/2); //nbl
    frustum[3] = nearCenter - up * (nearHeight/2) + right * (nearWidth/2); //nbr

    frustum[4] = farCenter + up * (farHeight/2) - right * (farWidth/2);    //ftl
    frustum[5] = farCenter + up * (farHeight/2) + right * (farWidth/2);    //ftr
    frustum[6] = farCenter - up * (farHeight/2) - right * (farWidth/2);    //fbl
    frustum[7] = farCenter - up * (farHeight/2) + right * (farWidth/2);    //fbr

    return frustum;
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
