#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include "Light.h"
#include <vector>
#include <cmath>
#include <QImage>
#include <memory>
#include "Math/Utility.h"

class PointLight : public Light{
public:

    enum ShadowMapFace{
        POSITIVE_X = 0,
        NEGATIVE_X = 1,
        POSITIVE_Y = 2,
        NEGATIVE_Y = 3,
        POSITIVE_Z = 4,
        NEGATIVE_Z = 5,
    };

    static ShadowMapFace pickFace(const Vector3& v){
        double ax = std::fabs(v.x), ay = std::fabs(v.y), az = std::fabs(v.z);
        if (ax > ay && ax > az) return (v.x >= 0 ? POSITIVE_X : NEGATIVE_X);
        if (ay > az)            return (v.y >= 0 ? POSITIVE_Y : NEGATIVE_Y);
        return                      (v.z >= 0 ? POSITIVE_Z : NEGATIVE_Z);
    }

    static constexpr double FOV = M_PI/2;
    static constexpr double ASPECT = 1.0;
    static constexpr int defaultShadowMapSize = 2048;

    double range = 400.0;
    double attenuationConstant = 1.0f;
    //double attenuationLinear = 0.09f;
    //double attenuationQuadratic = 0.032f;
    double attenuationLinear = 0.00f;
    double attenuationQuadratic = 0.000f;

    std::vector<std::shared_ptr<Buffer<double>>> shadowMaps;
    std::vector<Matrix4> viewMatrices = std::vector(6, Matrices4::identity());
    Matrix4 projectionMatrix = Matrices4::identity();

    PointLight()
    {
        shadowMaps.resize(6);
        for (int i = 0; i < 6; ++i) {
            shadowMaps[i] = std::make_shared<Buffer<double>>(defaultShadowMapSize, defaultShadowMapSize, std::numeric_limits<double>::infinity());
        }
        lightType = LightType::POINT;
        emitterRadiusWorld = 0.05;

    }

    double getWorldUnitsPerTexel(double depth){
        static constexpr double fovYTan = 1.0;
        const double fovX = 2*atan(fovYTan);
        const double fovXTan = tan(fovX/2.0);

        double tY = 2*depth*fovYTan/shadowMaps[0]->size();
        double tX = 2*depth*fovXTan/shadowMaps[0]->size(); //assuming aspect is 1, which it always is
        return std::max(tX,tY);
    }

    double getAttenuation(double distance){
        return distance <= range? 1.0/(attenuationConstant + attenuationLinear*distance + attenuationQuadratic*distance*distance) : 0.0;
    }

    void setViewMatrices(){

        const Vector3 L = transform.getPosition();
        // kanoniczne up-y; ważne – to one determinują orientację UV
        viewMatrices[POSITIVE_X] = LightMatrices::lightView(L, L + Vector3{+1, 0, 0}, Vector3{0, -1, 0});
        viewMatrices[NEGATIVE_X] = LightMatrices::lightView(L, L + Vector3{-1, 0, 0}, Vector3{0, -1, 0});
        viewMatrices[POSITIVE_Y] = LightMatrices::lightView(L, L + Vector3{0, +1, 0}, Vector3{0, 0, +1});
        viewMatrices[NEGATIVE_Y] = LightMatrices::lightView(L, L + Vector3{0, -1, 0}, Vector3{0, 0, -1});
        viewMatrices[POSITIVE_Z] = LightMatrices::lightView(L, L + Vector3{0, 0, +1}, Vector3{0, -1, 0});
        viewMatrices[NEGATIVE_Z] = LightMatrices::lightView(L, L + Vector3{0, 0, -1}, Vector3{0, -1, 0});
    }

    std::vector<Matrix4> getViewMatrices(){
        return viewMatrices;
    }

    Matrix4 getViewMatrix(int face){
        return viewMatrices[face];
    }

    void setProjectionMatrix(double near, double far){
        const double fovOffset = MathUt::degreeToRadian(0.5);
        projectionMatrix = LightMatrices::PerspectiveLightProjection(FOV+fovOffset , near, far, ASPECT);
    }

    Matrix4 getProjectionMatrix(){
        return projectionMatrix;
    }


    double normalizedDepthToWorld(double depth){
        return near + depth*(range - near);
    }

    void printShadowMaps(){
        for(int face=0; face<6; face++){
            QImage img(shadowMaps[face]->getCols(), shadowMaps[face]->getRows(), QImage::Format_Grayscale8);
            for (int y = 0; y < (int)shadowMaps[face]->getRows(); ++y) {
                for (int x = 0; x < (int)shadowMaps[face]->getCols(); ++x) {
                    double d = (*shadowMaps[face])[y][x];
                    // 0 = close, 1 = far, inf = empty
                    //int v = (d == std::numeric_limits<double>::infinity()) ? 0 : int(255 * (1.0 - d));
                    int v = (d == std::numeric_limits<double>::infinity()) ? 0 : 255;
                    img.setPixel(x, y, qRgb(v, v, v));
                }
            }
            img.save(QString("shadowmap_%1.png").arg(face));
        }
    }
};


#endif // POINTLIGHT_H
