#include "Math/Utility.h"
#include <algorithm>

bool MathUt::equal(double d1, double d2, double epsilon){
    return std::abs(d1 - d2) <= epsilon;
}

double MathUt::mean(std::vector<double> v){
    double result{};
    for(const auto& it : v){
        result+=it;
    }
    return result/v.size();
}

//i == 0 is illegal, should be validate before function call
int MathUt::OnetoZeroBased(int i, int size){
    if(i > 0) return i-1;
    else return size+i;
}

double MathUt::safeDenom(double denom, double safe){
    return denom == 0? safe : denom;
}

double MathUt::safePositiveDenom(double denom, double safe){
    return denom <= 0? safe : denom;
}

double MathUt::degreeToRadian(double degree){
    return (M_PI/180.0)*degree;
}

 double MathUt::radianToDegree(double radian){
    return (180.0/M_PI)*radian;
}

double MathUt::euclideanDistance(const Vector2& v1, const Vector2& v2){
    return std::sqrt( (v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));
}

float MathUt::smoothstep(float edge0, float edge1, float x) {
    float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

double MathUt::randomInRangeHard(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(min, std::nextafter(max, std::numeric_limits<double>::max()));
    return dist(gen);
}

double MathUt::randomInRangeHard(double min, double max, std::mt19937& gen) {
    std::uniform_real_distribution<double> dist(min, std::nextafter(max, std::numeric_limits<double>::max()));
    return dist(gen);
}

double MathUt::randomInRangeSoft(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(std::nextafter(min, std::numeric_limits<double>::max()), max);
    return dist(gen);
}

Vector2 MathUt::randomPointInDistance(Vector2 center, double minDist, double maxDist,
                              double minX, double maxX, double minY, double maxY)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<double> xdist(minX, maxX);
    std::uniform_real_distribution<double> ydist(minY, maxY);

    while (true) {
        double x = xdist(gen);
        double y = ydist(gen);
        double dist = std::hypot(x - center.x, y - center.y);
        if (dist >= minDist && dist <= maxDist)
            return {x, y};
    }
}

Vector2 MathUt::randomPointInRing(const Vector2& center, double r, std::mt19937& gen) {
    std::uniform_real_distribution<double> angleDist(0.0, 2.0*M_PI);
    std::uniform_real_distribution<double> rad2Dist(r*r, 4*r*r);
    double theta = angleDist(gen);
    double radius = std::sqrt(rad2Dist(gen));
    return { center.x + radius*std::cos(theta), center.y + radius*std::sin(theta) };
}

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

double RendUt::sRGBToLinear(double sRGB){
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
