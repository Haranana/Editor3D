#ifndef CAMERA_H
#define CAMERA_H

#include "Scene/Object3D.h"
#include "Math/Vectors.h"

class Camera : public  Object3D{
public:

    enum class CameraType{
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    Camera(int screenWidth , int screenHeight) : screenWidth(screenWidth) , screenHeight(screenHeight) {
        aspect = double(screenWidth)/double(screenHeight);
        oLeft = -screenWidth * 0.5;
        oRight =  screenWidth * 0.5;
        oBottom = -screenHeight * 0.5;
        oTop = screenHeight * 0.5;
    }

    Matrix4 getViewMatrix();
    Matrix4 getProjectionMatrix();
    void setPerspective(double fovY, double aspect, double nearPlane, double farPlane);
    std::vector<Vector3> getFrustum();

    CameraType cameraType = CameraType::PERSPECTIVE;
    double fovY = defaultFovY;
    double aspect = 1.0;
    double nearPlane = defaultNearPlane;
    double farPlane  = defaultFarPlane;

private:
    static constexpr double defaultFovY = 60.0;
    static constexpr double defaultNearPlane = 0.1;
    static constexpr double defaultFarPlane = 1000.0;
    const int screenWidth;
    const int screenHeight;
    double oRight;
    double oLeft;
    double oTop;
    double oBottom;

};

#endif // CAMERA_H
