#include "Rendering/Utility.h"
#include <cmath>

Vector3 RendUt::sRGBToLinear(const Vector3& sRGB){
    Vector3 linear;

    if (sRGB.x <= 0.04045){
        linear.x =  sRGB.x / 12.92;
    }
    else{
        linear.x = std::pow((sRGB.x + 0.055) / 1.055, 2.4);
    }

    if (sRGB.y <= 0.04045){
        linear.y =  sRGB.y / 12.92;
    }
    else{
        linear.y = std::pow((sRGB.y + 0.055) / 1.055, 2.4);
    }

    if (sRGB.z <= 0.04045){
        linear.z =  sRGB.z / 12.92;
    }
    else{
        linear.z = std::pow((sRGB.z + 0.055) / 1.055, 2.4);
    }

    return linear;
}

double sRGBToLinear(double sRGB){
    double linear;

    if (sRGB <= 0.04045){
        linear =  sRGB / 12.92;
    }
    else{
        linear = std::pow((sRGB + 0.055) / 1.055, 2.4);
    }
    return std::clamp(linear, 0.0, 1.0);
}


Vector3 RendUt::linearToSRGB(const Vector3& linear){
    Vector3 sRGB;
    Vector3 clampedLinear(linear);

    clampedLinear.x = std::clamp(linear.x, 0.0, 1.0);
    clampedLinear.y = std::clamp(linear.y, 0.0, 1.0);
    clampedLinear.z = std::clamp(linear.z, 0.0, 1.0);

    if (clampedLinear.x <= 0.0031308){
       sRGB.x = 12.92 * clampedLinear.x;
    }else{
        sRGB.x = 1.055 * std::pow(clampedLinear.x, 1.0 / 2.4) - 0.055;
    }

    if (clampedLinear.y <= 0.0031308){
        sRGB.y = 12.92 * clampedLinear.y;
    }else{
        sRGB.y = 1.055 * std::pow(clampedLinear.y, 1.0 / 2.4) - 0.055;
    }


    if (clampedLinear.z <= 0.0031308){
        sRGB.z = 12.92 * clampedLinear.z;
    }else{
        sRGB.z = 1.055 * std::pow(clampedLinear.z, 1.0 / 2.4) - 0.055;
    }

    return sRGB;
}
