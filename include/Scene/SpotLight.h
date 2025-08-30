#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H
#include "Light.h"
#include <cmath>
#include "Math/Utility.h"

class SpotLight : public Light{
public:

    Matrix4 getViewMatrix() const{
        return viewMatrix;
    }

    void setViewMatrix(Vector3 up = defaultUp){
        const Vector3 normDir = direction.normalize();
        viewMatrix =  LightMatrices::lightView(this->transform.getPosition(), this->transform.getPosition() + normDir, up);
    }

    void setProjectionMatrix(double near, double far){
        projectionMatrix = LightMatrices::PerspectiveLightProjection(outerAngle*2 , near, far, ASPECT);
    }


    Matrix4 getProjectionMatrix(){
        return projectionMatrix;
    }



    static constexpr int defaultShadowMapSize = 2048;
    static constexpr double ASPECT = 1.0;
    static constexpr Vector3 defaultUp = Vector3(0.0,1.0,0.0);

    static constexpr double defaultAttConstSmall = 1.0;
    static constexpr double defaultAttConstMedium = 1.0;
    static constexpr double defaultAttConstLarge = 1.0;

    static constexpr double defaultAttLinearSmall = 0.7;
    static constexpr double defaultAttLinearMedium = 0.22;
    static constexpr double defaultAttLinearLarge = 0.9;

    static constexpr double defaultAttQuadSmall = 1.8;
    static constexpr double defaultAttQuadMedium = 0.2;
    static constexpr double defaultAttQuadLarge = 0.032;

    static constexpr double deafultRange = 500.0;
    static constexpr double defaultOuterAngle = 1.047; //60 degrees
    static constexpr double defaultInnerAngle = 0.785; //45 degrees
    static constexpr Vector3 defaultDirection = Vector3(1.0,0.0,0.0);

    Vector3 direction = defaultDirection;

    Buffer<double> shadowMap;

    double outerAngle = defaultOuterAngle; //default 60 degrees
    double innerAngle = defaultInnerAngle; //default 45 degrees
    double range = deafultRange;

    double attenuationConstant = 1.0;
    double attenuationLinear = 0.0;
    double attenuationQuadratic = 0.0;

    SpotLight() : shadowMap(defaultShadowMapSize, defaultShadowMapSize, std::numeric_limits<double>::infinity())
    {
        lightType = LightType::SPOT;
        emitterRadiusWorld = 0.05;
    }

    double getWorldUnitsPerTexel(double depth){
        const double fovY = outerAngle*2;
        const double fovYTan = tan(fovY/2.0);
        const double fovX = 2*atan(fovYTan);
        const double fovXTan = tan(fovX/2.0);

        double tY = 2 * depth * fovYTan/defaultShadowMapSize;
        double tX = 2*depth*fovXTan/defaultShadowMapSize;
        return std::max(tX,tY);
    }

    double getConeAttenuation(const Vector3& lightToPoint){

        double outerAngleCos = getOuterAngleCos();
        double innerAngleCos = getInnerAngleCos();
        Vector3 normDir = direction.normalize();
        Vector3 lightToPointDir = lightToPoint.normalize();
        double lightToPointCos = (lightToPointDir).dotProduct(normDir);

        double denom = MathUt::safePositiveDenom(innerAngleCos - outerAngleCos);
        return std::clamp( ((lightToPointCos-outerAngleCos)/denom), 0.0, 1.0);

    }

    double normalizedDepthToWorld(double depth){
        return near + depth*(range - near);
    }

    double getDistanceAttenuation(double distance){
        return 1/(attenuationConstant + attenuationLinear*distance + attenuationQuadratic*distance*distance);
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
