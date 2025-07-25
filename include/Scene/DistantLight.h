#ifndef DISTANTLIGHT_H
#define DISTANTLIGHT_H

#include "Light.h"
#include "limits.h"
#include <QImage>
//despite having position because of inheritance DistantLight<-Light<-Object3D
//its position should never be accessed due to definiton of distant light only its direction should ever be used
//in any calculatins
//
class DistantLight : public Light{
public:

    static constexpr Vector3 defaultUp = Vector3(0,0,1);
    static constexpr Vector3 secondChoiceUp = Vector3(0,0,1);
    static constexpr size_t defaultShadowMapSize = 1024; //in pixels

    //probably should be kept normalized
    Vector3 direction;
    Buffer<double> shadowMap;

    DistantLight(Vector3 direction) :
        direction(direction.normalize()) ,
        shadowMap(defaultShadowMapSize, defaultShadowMapSize, std::numeric_limits<double>::infinity())
    {}

    void printShadowMatrix(){
        QImage img(shadowMap.getCols(), shadowMap.getRows(), QImage::Format_Grayscale8);
        for (int y = 0; y < shadowMap.getRows(); ++y) {
            for (int x = 0; x < shadowMap.getCols(); ++x) {
                float d = shadowMap[y][x];
                // 0 = close, 1 = far, inf = empty
                int v = (d == std::numeric_limits<float>::infinity()) ? 0 : int(255 * (1.0 - d));
                img.setPixel(x, y, qRgb(v, v, v));
            }
        }
        img.save("shadowmap.png");
    }

    void setViewMatrix(const Vector3& bboxCenter, Vector3 up = defaultUp){
        if(up.isParallel(direction)) up = secondChoiceUp;
        viewMatrix = LightMatrices::lightView(bboxCenter, bboxCenter+direction, up);
    }

    Matrix4 getViewMatrix(){
        return viewMatrix;
    };

    void setProjectionMatrix(double orthoLeft,
                             double orthoRight,
                             double orthoBottom,
                             double orthoTop,
                             double orthoNear,
                             double orthoFar){

        projectionMatrix = LightMatrices::orthogonalLightProjection(orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar);
    }

    Matrix4 getProjectionMatrix(){
        return projectionMatrix;
    };

    //should get 8 vertices as an argument
    Vector3 getBBoxCenter(const std::vector<Vector3>& boundingBox){
        double meanX = 0.0;
        double meanY = 0.0;
        double meanZ = 0.0;

        for(const Vector3& v: boundingBox){
            meanX = meanX + v.x;
            meanY = meanY + v.y;
            meanZ = meanZ + v.z;
        }
        return Vector3(meanX/boundingBox.size() , meanY/boundingBox.size(), meanZ/boundingBox.size());
    }

    Matrix4 viewMatrix = Matrices4::identity();;
    Matrix4 projectionMatrix = Matrices4::identity();


};

#endif // DISTANTLIGHT_H
