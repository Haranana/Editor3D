#ifndef UTILITY_H
#define UTILITY_H

#include <Math/Vectors.h>
#include <random>
#include <cmath>

namespace MathUt{

bool equal(double d1, double d2, double epsilon = 1e-6);

int OnetoZeroBased(int i, int size);

double safeDenom(double denom, double safe = 1e-6);

double safePositiveDenom(double denom, double safe = 1e-6);

double degreeToRadian(double degree);

double radianToDegree(double radian);

double euclideanDistance(const Vector2& v1, const Vector2& v2);

float smoothstep(float edge0, float edge1, float x);

double randomInRangeHard(double min, double max);

double randomInRangeHard(double min, double max, std::mt19937& gen);

double randomInRangeSoft(double min, double max);

Vector2 randomPointInDistance(Vector2 center, double minDist, double maxDist,
                              double minX, double maxX, double minY, double maxY);

Vector2 randomPointInRing(const Vector2& center, double r, std::mt19937& gen);

}

//any color conversions assume BGR standard, same as most things in engine
//magical numbers in sRGB->linear(and reverse) are from IEC 61966-2-1
namespace RendUt{

Vector3 sRGBToLinear(const Vector3& sRGB);
double sRGBToLinear(double sRGB);
Vector3 linearToSRGB(const Vector3& linear);


}
#endif // UTILITY_H
