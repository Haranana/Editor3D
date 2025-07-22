#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include "Light.h"
#include <array>
#include <vector>
#include <cmath>

class PointLight : public Light{

    double attenuationConstant = 1.0f;
    double attenuationLinear = 0.0f;
    double attenuationQuadratic = 0.0f;
    double range = 50.0f;

    static constexpr double FOV = M_PI/2;
    static constexpr double ASPECT = 1.0;
    static constexpr int defaultShadowMapSize = 512;

    std::vector<std::shared_ptr<Buffer<double>>> shadowMaps;


    Matrix4 getViewMatrix(int face){
        static const Vector3 dirs[6] = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1} };
        static const Vector3 ups[6]  = { {0,-1,0},{0,-1,0},{0,0,1},{0,0,-1},{0,-1,0},{0,-1,0} };
        return LightMatrices::lightView(this->transform.getPosition(), this->transform.getPosition() + dirs[face], ups[face]);
    }

    Matrix4 getProjectionMatrix(double near, double far){
        return LightMatrices::PerspectiveLightProjection(FOV , near, far, ASPECT);
    }

};


#endif // POINTLIGHT_H
