#ifndef RENDERING_UTILITY_H
#define RENDERING_UTILITY_H
#include "Color.h"
#include "Math/Vector3.h"

//any color conversions assume BGR standard, same as most things in engine
//magical numbers in sRGB->linear(and reverse) are from IEC 61966-2-1
namespace RendUt{

Vector3 sRGBToLinear(const Vector3& sRGB);
double sRGBToLinear(double sRGB);

Vector3 linearToSRGB(const Vector3& linear);
//Color sRGBToLinear(const Color& sRGB);
//Color linearToSRGB(const Color& linear);

}

#endif // RENDERING_UTILITY_H
