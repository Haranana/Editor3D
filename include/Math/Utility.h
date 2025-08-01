#ifndef UTILITY_H
#define UTILITY_H
#include <cmath>
namespace MathUt{
    constexpr double degreeToRadian(double degree){
        return (M_PI/180)*degree;
    }

    constexpr double radianToDegree(double radian){
        return (180/M_PI)*radian;
    }
};
#endif // UTILITY_H
