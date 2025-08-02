#ifndef UTILITY_H
#define UTILITY_H
#include <algorithm>
#include <cmath>
namespace MathUt{
    constexpr double degreeToRadian(double degree){
        return (M_PI/180)*degree;
    }

    constexpr double radianToDegree(double radian){
        return (180/M_PI)*radian;
    }

    constexpr float smoothstep(float edge0, float edge1, float x) {
        float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
};
#endif // UTILITY_H
