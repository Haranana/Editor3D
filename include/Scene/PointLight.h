#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include "Light.h"
#include <array>
#include <vector>
#include <cmath>

class PointLight : public Light{
public:
    static constexpr double FOV = M_PI/2;
    static constexpr double ASPECT = 1.0;
    static constexpr int defaultShadowMapSize = 512;


    double range = 100.0;
    double attenuationConstant = 1.0f;
    double attenuationLinear = 0.0f;
    double attenuationQuadratic = 0.0f;

    std::vector<std::shared_ptr<Buffer<double>>> shadowMaps;
    std::vector<Matrix4> viewMatrices = std::vector(6, Matrices4::identity());
    Matrix4 projectionMatrix = Matrices4::identity();

    void setViewMatrices(){

        static const Vector3 dirs[6] = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1} }; //+X, -X, +Y, -Y, +Z, -Z
        static const Vector3 ups[6]  = { {0,1,0},{0,1,0},{0,0,1},{0,0,1},{1,0,0},{1,0,0} };    //+Y, +Y, +Z, +Z, +X, +X

        for(size_t face = 0; face < 6; face++){
            viewMatrices[face] = LightMatrices::lightView(this->transform.getPosition(), (this->transform.getPosition() + dirs[face]).normalize(), ups[face]);
        }
    }

    std::vector<Matrix4> getViewMatrices(){
        return viewMatrices;
    }

    Matrix4 getViewMatrix(int face){
        return viewMatrices[face];
    }

    void setProjectionMatrix(double near, double far){
        projectionMatrix = LightMatrices::PerspectiveLightProjection(FOV , near, far, ASPECT);
    }

    Matrix4 getProjectionMatrix(){
        return projectionMatrix;
    }

};


#endif // POINTLIGHT_H
