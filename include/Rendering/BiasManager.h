#ifndef BIASMANAGER_H
#define BIASMANAGER_H
#include "Buffer.h"
#include "Math/Vector3.h"
#include "Math/Utility.h"
#include <cmath>
class BiasManager{
public:


    double getSlopeScaled(const Buffer<double>& shadowMap,
                         const Vector3& p0, const Vector3& p1, const Vector3& p2,
                         double alphaConst = 1.5,int pcfKernelSize = 0){
        double kSlope = getSsdbKSlope();
        double slope = getSsdbSlope(p0,p1,p2);
        double kConst = getSsdbKConst(alphaConst, pcfKernelSize, int(std::min(shadowMap.getRows() , shadowMap.getCols())));

        return kSlope * slope + kConst;
    }

private:
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

};

#endif // BIASMANAGER_H
