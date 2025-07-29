#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include "Light.h"
#include <array>
#include <vector>
#include <cmath>
#include <QImage>
#include <memory>

class PointLight : public Light{
public:
    static constexpr double FOV = M_PI/2;
    static constexpr double ASPECT = 1.0;
    static constexpr int defaultShadowMapSize = 512;

    double range = 100.0;
    double attenuationConstant = 1.0f;
    double attenuationLinear = 0.09f;
    double attenuationQuadratic = 0.032f;

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

    }

    double getAttenuation(double distance){
        return distance <= range? 1.0/(attenuationConstant + attenuationLinear*distance + attenuationQuadratic*distance*distance) : 0.0;
    }

    void setViewMatrices(){

        static const Vector3 dirs[6] = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1} }; //+X, -X, +Y, -Y, +Z, -Z
        static const Vector3 ups[6]  = { {0,1,0},{0,1,0},{0,0,1},{0,0,1},{1,0,0},{1,0,0} };    //+Y, +Y, +Z, +Z, +X, +X

        for(size_t face = 0; face < 6; face++){
            viewMatrices[face] = LightMatrices::lightView(this->transform.getPosition(), (this->transform.getPosition() + dirs[face]), ups[face]);
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

    void printShadowMaps(){
        for(int face=0; face<6; face++){
            QImage img(shadowMaps[face]->getCols(), shadowMaps[face]->getRows(), QImage::Format_Grayscale8);
            for (int y = 0; y < shadowMaps[face]->getRows(); ++y) {
                for (int x = 0; x < shadowMaps[face]->getCols(); ++x) {
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
