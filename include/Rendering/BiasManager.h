#ifndef BIASMANAGER_H
#define BIASMANAGER_H
#include "Buffer.h"
#include "Math/Vector3.h"
#include "Math/Utility.h"
#include <cmath>
class BiasManager{
public:

    //p0,p1,p2 are triangle vertices and should be in lightSpace
    double getSlopeScaled(const Buffer<double>& shadowMap,
                         const Vector3& p0, const Vector3& p1, const Vector3& p2,
                         double alphaConst = 1.5,int pcfKernelSize = 0){
        double kSlope = getSsdbKSlope();
        double slope = getSsdbSlope(p0,p1,p2);
        double kConst = getSsdbKConst(alphaConst, pcfKernelSize, int(std::min(shadowMap.getRows() , shadowMap.getCols())));

        return kSlope * slope + kConst;
    }

private:

    //SSDB
    double getSsdbKSlope(){
        return defaultSsdbKSlope;
    }

    double getSsdbSlope(const Vector3& p0, const Vector3& p1, const Vector3& p2){
        Vector3 e1{p1 - p0};
        Vector3 e2{p2-p0};

        double denom = e1.x*e2.y - e1.y*e2.x;
        if(MathUt::equal(denom , 0.0)) return 0.0; //no slope

        double a = (e1.z*e2.y - e2.z*e1.y) / denom;
        double b = (-e1.z*e2.x + e2.z*e1.x) / denom;

        return std::max(abs(a) , abs(b));
    }

    double getSsdbKConst(double alphaConst,int pcfKernelSize,int minHeightWidth){
        return alphaConst*std::max(1, pcfKernelSize)/minHeightWidth;
    }


    static constexpr double defaultSsdbKSlope = 1.0;


    //Normal Angle
    double getNaNormalFactor(){
        return defaultNaNormalFactor;
    }

    double getNaAngleFactor(){
        return defaultNaAngleFactor;
    }

    double getNaWorldUnitsPerTexelDistantLight(double right, double left, double top, double bottom){
        return std::max( (right-left) , (top-bottom));
    }

    //in point light FovY = 90, tan(45) = 1
    double getNaWorldUnitsPerTexelPointLight(double depth, int shadowMapWidth, int shadowMapHeight){
        static constexpr double angleTan = 1.0;
        double tY = 2*depth*angleTan/shadowMapHeight;
        double tX = 2*depth*angleTan/shadowMapWidth; //assuming aspect is 1, which it always is
        return std::max(tX,tY);
    }

    double getNaWorldUnitsPerTexelSpotLight(){

    }

    double getNaPCFCorrectOnWorldUnitsPerTexel(double wupt, int kernel){
        return wupt*(double(std::max(1 , kernel)));
    }

    double getNaAngleBias(const Vector3& normal, const Vector3& lightVector){
        return (1-std::max(0.0 , normal.dotProduct(lightVector)) );
    }

    static constexpr double defaultNaNormalFactor = 0.75;
    static constexpr double defaultNaAngleFactor = 1.0;


};

#endif // BIASMANAGER_H
