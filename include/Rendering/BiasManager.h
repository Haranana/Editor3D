#ifndef BIASMANAGER_H
#define BIASMANAGER_H
#include "Buffer.h"
#include "Math/Vector3.h"
#include "Math/Utility.h"
#include "Scene/DistantLight.h"
#include "Scene/SpotLight.h"
#include "Scene/PointLight.h"
#include "Math/Vectors.h"
#include <cmath>


class BiasManager{
public:

    //p0,p1,p2 are triangle vertices and should be in lightSpace
    static double getSlopeScaled(const Buffer<double>& shadowMap,
                         const Vector3& p0, const Vector3& p1, const Vector3& p2,
                         int pcfKernelSize = 0, double alphaConst = 1.5){ //for aplpha cont light const bias is also recommended
        double kSlope = getSsdbKSlope();
        double slope = getSsdbSlope(p0,p1,p2);
        double kConst = getSsdbKConst(alphaConst, pcfKernelSize, int(std::min(shadowMap.getRows() , shadowMap.getCols())));

        return kSlope * slope + kConst;
    }

    static Vector3 getNormalAngleDistant(const Buffer<double>& shadowMap,
                                 const DistantLight& light,
                                 const Vector3& normal,
                                 const Vector3& point,
                                 int pcfKernelSize = 0){

        double worldUnits = pcfCorrect(getNaWorldUnitsPerTexelDistantLight(shadowMap,
                                light.oRight,light.oLeft,light.oTop, light.oBottom)
                            , pcfKernelSize);
        Vector3 pointToLightVector = (light.direction*(-1.0)).normalize();

        double angleFactors = (1+getNaAngleFactor()*getNaAngleBias(normal, pointToLightVector));
        double moveDistance = getNaNormalFactor()*worldUnits*angleFactors;

        return (normal*moveDistance)+point;
    }

    static Vector3 getNormalAnglePoint(const Buffer<double>& shadowMap,
                                const PointLight& light,
                                int face,
                                const Vector3& normal,
                                const Vector3& pointToLightVector,
                                const Vector3& point,
                                int pcfKernelSize = 0){

        Vector4 pointInLightView = (light.viewMatrices[face] * Vectors::vector3to4(point));
        double zLS = -pointInLightView.z;

        double worldUnits = pcfCorrect(getNaWorldUnitsPerTexelPointLight(zLS , shadowMap.getCols(), shadowMap.getRows()), pcfKernelSize);

        double angleFactors = (1+getNaAngleFactor()*getNaAngleBias(normal, pointToLightVector));
        double moveDistance = getNaNormalFactor()*worldUnits*angleFactors;

        return (normal*moveDistance)+point;
    }

    static Vector3 getNormalAngleSpot(const Buffer<double>& shadowMap,
                               const SpotLight& light,
                               const Vector3& normal,
                               const Vector3& pointToLightVector,
                               const Vector3& point,
                               int pcfKernelSize = 0){
        double fovY = light.outerAngle*2;

        Vector4 pointInLightView = (light.getViewMatrix() * Vectors::vector3to4(point));
        double zLS = -pointInLightView.z;

        double worldUnits = pcfCorrect(getNaWorldUnitsPerTexelSpotLight(fovY , zLS, shadowMap.getCols(), shadowMap.getRows()), pcfKernelSize);
        double angleFactors = (1+getNaAngleFactor()*getNaAngleBias(normal, pointToLightVector));
        double moveDistance = getNaNormalFactor()*worldUnits*angleFactors;

        return (normal*moveDistance)+point;
    }

    //calculate small bias dependand on texel size, used as bias addition in Normal angle
    static double getMinBias(const Buffer<double>& shadowMap){
        constexpr static double biasBase = 1e-4;
        const double texelZ = 1.0 / std::max(1, std::min((int)shadowMap.width(), (int)shadowMap.height()));
        return std::max(biasBase, 0.5*texelZ);
    }

private:

    //SSDB
    static double getSsdbKSlope(){
        return defaultSsdbKSlope;
    }

    static double getSsdbSlope(const Vector3& p0, const Vector3& p1, const Vector3& p2){
        Vector3 e1{p1 - p0};
        Vector3 e2{p2-p0};

        double denom = e1.x*e2.y - e1.y*e2.x;
        if(MathUt::equal(denom , 0.0 , 1e-12)){ //denom = 0 => very big slope, return huge number and clamp it
            return 1e6;
        }

        double a = (e1.z*e2.y - e2.z*e1.y) / denom;
        double b = (-e1.z*e2.x + e2.z*e1.x) / denom;

        return std::max(std::abs(a) , std::abs(b));
    }

    static double getSsdbKConst(double alphaConst,int pcfKernelSize,int minHeightWidth){
        if (minHeightWidth <= 0) return 0.0;
        if (pcfKernelSize <= 1)  return alphaConst / double(minHeightWidth);
        const double r = RendUt::kernelRadiusFromSide(pcfKernelSize);
        return alphaConst * r / double(minHeightWidth);
    }

    static constexpr double defaultSsdbKSlope = 1.0;


    //Normal Angle


    static double getNaNormalFactor(){
        return defaultNaNormalFactor;
    }

    static double getNaAngleFactor(){
        return defaultNaAngleFactor;
    }

    static double getNaWorldUnitsPerTexelDistantLight(const Buffer<double>& shadowMap, double right, double left, double top, double bottom){
        return std::max( (right-left)/shadowMap.width() , (top-bottom)/shadowMap.height());
    }

    //in point light FovY = 90, tan(45) = 1
    static double getNaWorldUnitsPerTexelPointLight(double depth, int shadowMapWidth, int shadowMapHeight){
        static constexpr double fovYTan = 1.0;
        const double fovX = 2*atan(fovYTan);
        const double fovXTan = tan(fovX/2.0);

        double tY = 2*depth*fovYTan/shadowMapHeight;
        double tX = 2*depth*fovXTan/shadowMapWidth; //assuming aspect is 1, which it always is
        return std::max(tX,tY);
    }

    static double getNaWorldUnitsPerTexelSpotLight(double fovY, double depth, int shadowMapWidth, int shadowMapHeight){

        const double fovYTan = tan(fovY/2.0);
        const double fovX = 2*atan(fovYTan);
        const double fovXTan = tan(fovX/2.0);

        double tY = 2 * depth * fovYTan/shadowMapHeight;
        double tX = 2*depth*fovXTan/shadowMapWidth;
        return std::max(tX,tY);
    }

    static double pcfCorrect(double wupt, int kernel){
        if (kernel <= 1) return wupt;
        const double r = RendUt::kernelRadiusFromSide(kernel);
        return wupt * r;
    }

    static double getNaAngleBias(const Vector3& normal, const Vector3& lightVector){
        Vector3 normalizedNormal = normal.normalize();
        Vector3 normalizedLightVector3 = lightVector.normalize();
        return (1-std::max(0.0 , normalizedNormal.dotProduct(normalizedLightVector3)));
    }



    static constexpr double defaultNaNormalFactor = 0.75;
    static constexpr double defaultNaAngleFactor = 1.0;


};

#endif // BIASMANAGER_H
