#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H
#include "Light.h"
#include <cmath>

class SpotLight : public Light{
public:
    Vector3 direction = Vector3(1.0,0.0,0.0);
    double outerAngle = 1.047; //default 60 degrees
    double innerAngle = 0.785; //default 45 degrees
    double range = 200.0;

    double attenuationConstant = 1.0f;
    double attenuationLinear = 0.0f;
    double attenuationQuadratic = 0.0f;

    Buffer<double> shadowMap;
    static constexpr int defaultShadowMapSize = 512;
    static constexpr double ASPECT = 1.0;
    static constexpr Vector3 defaultUp = Vector3(0.0,1.0,0.0);

    SpotLight() : shadowMap(defaultShadowMapSize, defaultShadowMapSize, std::numeric_limits<double>::infinity())
    {
        lightType = LightType::SPOT;
    }

    //light Point needs to be normalized!
    double getAttenuation(const Vector3& lightToPoint){
        double outerAngleCos = getOuterAngleCos();
        double innerAngleCos = getInnerAngleCos();

        return (direction.dotProduct(lightToPoint) - outerAngleCos)/(innerAngleCos - outerAngleCos);
    }

    double getDistanceAttenuation(double distance){
        return 1/(attenuationConstant + attenuationLinear*distance + attenuationQuadratic*distance*distance);
    }

    void setViewMatrix(Vector3 up = defaultUp){
        viewMatrix =  LightMatrices::lightView(this->transform.getPosition(), this->transform.getPosition() + direction, up);
    }

    void setProjectionMatrix(double near, double far){
        projectionMatrix = LightMatrices::PerspectiveLightProjection(outerAngle*2 , near, far, ASPECT);
    }

    Matrix4 getViewMatrix(){
        return viewMatrix;
    }

    Matrix4 getProjectionMatrix(){
        return projectionMatrix;
    }

private:

    double getOuterAngleCos(){
        return std::cos(outerAngle);
    }

    double getInnerAngleCos(){
        return std::cos(innerAngle);
    }

    Matrix4 viewMatrix = Matrices4::identity();;
    Matrix4 projectionMatrix = Matrices4::identity();

};

#endif // SPOTLIGHT_H
