#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H
#include "Light.h"
#include <cmath>
#include <QImage>
#include "Math/Utility.h"

class SpotLight : public Light{
public:

    SpotLight() : shadowMap(defaultShadowMapSize, defaultShadowMapSize, std::numeric_limits<double>::infinity())
    {
        direction = defaultDirection;
        lightType = LightType::SPOT;
        emitterRadiusWorld = 0.05;
        range = deafultRange;
    }

    void setShadowMapSize(size_t newSize) override {
        shadowMap.setSize(newSize, newSize);
    }

    double getWorldUnitsPerTexel(double depth) const{
        const double fovY = outerAngle*2;
        const double fovYTan = tan(fovY/2.0);
        const double fovX = 2*atan(fovYTan);
        const double fovXTan = tan(fovX/2.0);

        double tY = 2 * depth * fovYTan/shadowMap.width();
        double tX = 2*depth*fovXTan/shadowMap.height();
        return std::max(tX,tY);
    }


    double getAttenuation(const Vector3& lightToPoint) override{
        double distance = lightToPoint.length();
        if(distance > range) return 0.0;


        return getConeAttenuation(lightToPoint.normalize().dotProduct(direction.normalize())) * getDistanceAttenuation(distance);
    }
    /*
    double getConeAttenuation(const Vector3& lightToPoint){

        Vector3 normDir = direction.normalize();
        Vector3 lightToPointDir = lightToPoint.normalize();
        double lightToPointCos = (lightToPointDir).dotProduct(normDir);

        double denom = MathUt::safePositiveDenom(InnerAngleCos - outerAngleCos);
        return std::clamp( ((lightToPointCos-outerAngleCos)/denom), 0.0, 1.0);
    }*/

    void updateCos(){
        outerAngleCos = std::cos(outerAngle);
        InnerAngleCos = std::cos(innerAngle);
        invCos = 1.0 / MathUt::safePositiveDenom(InnerAngleCos-outerAngleCos);
    }

    Buffer<double>& getShadowMap(size_t index = 0) override{
        return shadowMap;
    }

    const Buffer<double>& getShadowMap(size_t index = 0) const override{
        return shadowMap;
    }

    Matrix4 getViewMatrix(size_t index = 0) const override {
        return viewMatrix;
    }

    void setViewMatrix(Vector3 up = defaultUp){
        const Vector3 normDir = direction.normalize();
        viewMatrix =  LightMatrices::lightView(this->transform.getPosition(), this->transform.getPosition() + normDir, up);
    }

    void setProjectionMatrix(double near, double far){
        projectionMatrix = LightMatrices::PerspectiveLightProjection(outerAngle*2 , near, far, ASPECT);
    }


    Matrix4 getProjectionMatrix() const override{
        return projectionMatrix;
    }

    double normalizedDepthToWorld(double depth) const{
        return near + depth*(range - near);
    }

    void printShadowMatrix(){
        QImage img(shadowMap.getCols(), shadowMap.getRows(), QImage::Format_Grayscale8);
        for (int y = 0; y < (int)shadowMap.getRows(); ++y) {
            for (int x = 0; x < (int)shadowMap.getCols(); ++x) {
                float d = shadowMap[y][x];
                // 0 = close, 1 = far, inf = empty
                int v = (d == std::numeric_limits<float>::infinity()) ? 0 : int(255 * (1.0 - d));
                img.setPixel(x, y, qRgb(v, v, v));
            }
        }
        img.save("shadowmap_spot.png");
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

    double outerAngle = defaultOuterAngle; //default 60 degrees
    double innerAngle = defaultInnerAngle; //default 45 degrees

    Buffer<double> shadowMap;

    double outerAngleCos = std::cos(defaultOuterAngle);
    double InnerAngleCos = std::cos(defaultInnerAngle);
    double invCos = 1.0 / (std::cos(defaultInnerAngle)-std::cos(defaultOuterAngle));

    double attenuationConstant = 1.0;
    double attenuationLinear = 0.0;
    double attenuationQuadratic = 0.0;

private:

    double getOuterAngleCos(){
        return std::cos(outerAngle);
    }

    double getInnerAngleCos(){
        return std::cos(innerAngle);
    }

    double getConeAttenuation(double LightToPointDotDir){
        double cosTheta = LightToPointDotDir;
        if(cosTheta <= outerAngleCos){
            return 0.0;
        }else if(cosTheta >= InnerAngleCos){
            return 1.0;
        }else{
            return (cosTheta - outerAngleCos)* invCos;
        }
    }

    double getDistanceAttenuation(double distance){
        return 1/(attenuationConstant + attenuationLinear*distance + attenuationQuadratic*distance*distance);
    }

    Matrix4 viewMatrix = Matrices4::identity();;
    Matrix4 projectionMatrix = Matrices4::identity();

};

#endif // SPOTLIGHT_H
