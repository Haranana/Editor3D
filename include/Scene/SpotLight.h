#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H
#include "Light.h"

class SpotLight : public Light{
    Vector3 direction;
    double cutoffAngle;
    double innerAngle;

    double attenuationConstant = 1.0f;
    double attenuationLinear = 0.0f;
    double attenuationQuadratic = 0.0f;

    static constexpr int defaultShadowMapSize = 512;
    static constexpr double ASPECT = 1.0;
    static constexpr Vector3 defaultUp = Vector3(0.0,1.0,0.0);

    Matrix4 getViewMatrix(int face){
        static const Vector3 dir = direction;
        static const Vector3 up = defaultUp;
        return LightMatrices::lightView(this->transform.getPosition(), this->transform.getPosition() + direction, up);
    }

    Matrix4 getProjectionMatrix(double near, double far){
        return LightMatrices::PerspectiveLightProjection(cutoffAngle*2 , near, far, ASPECT);
    }

};

#endif // SPOTLIGHT_H
