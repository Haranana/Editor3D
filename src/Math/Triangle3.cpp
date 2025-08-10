#include "Math/Triangle3.h"
#include <algorithm>

bool Triangle3::isInTriangle2D(const Vector3& v){
    Triangle<double> barCoords = baricentricCoords(v);
    constexpr double eps = 1e-7;
    return barCoords.v1 >= -eps && barCoords.v2 >= -eps && barCoords.v3 >= -eps;

}

bool Triangle3::isInTriangle2D(const Vector2& v){
    Triangle<double> barCoords = baricentricCoords(v);
    constexpr double eps = 1e-7;
    return barCoords.v1 >= -eps && barCoords.v2 >= -eps && barCoords.v3 >= -eps;

}


double Triangle3::area(){
    return (v2.x - v1.x)*(v3.y - v1.y)
           - (v2.y - v1.y)*(v3.x - v1.x);
}


Triangle<double> Triangle3::baricentricCoords(const Vector3& v){
    double invArea = 1.0/area();
    double w0 = ((v2.x-v.x)*(v3.y-v.y) - (v2.y-v.y)*(v3.x-v.x)) * invArea;
    double w1 = ((v3.x-v.x)*(v1.y-v.y) - (v3.y-v.y)*(v1.x-v.x)) * invArea;
    double w2 = 1.0 - w0 - w1;

    return Triangle<double>(w0, w1, w2);
}

Triangle<double> Triangle3::baricentricCoords(const Vector2& v){
    double invArea = 1.0/area();
    double w0 = ((v2.x-v.x)*(v3.y-v.y) - (v2.y-v.y)*(v3.x-v.x)) * invArea;
    double w1 = ((v3.x-v.x)*(v1.y-v.y) - (v3.y-v.y)*(v1.x-v.x)) * invArea;
    double w2 = 1.0 - w0 - w1;

    return Triangle<double>(w0, w1, w2);
}

std::pair<int, int> Triangle3::getBorderX(int lowest, int highest){
    int minX = std::max(lowest, static_cast<int>(std::floor(std::min({v1.x, v2.x, v3.x}))));
    int maxX = std::min(highest - 1, static_cast<int>(std::ceil(std::max({v1.x, v2.x, v3.x}))));
    return {minX, maxX};

}

std::pair<int, int> Triangle3::getBorderY(int lowest, int highest){
    int minY = std::max(lowest, static_cast<int>(std::floor(std::min({v1.y, v2.y, v3.y}))));
    int maxY = std::min(highest - 1, static_cast<int>(std::ceil(std::max({v1.y, v2.y, v3.y}))));
    return {minY, maxY};
}

std::pair<int, int> Triangle3::getBorderZ(int lowest, int highest){
    int minZ = std::max(lowest, static_cast<int>(std::floor(std::min({v1.z, v2.z, v3.z}))));
    int maxZ = std::min(highest - 1, static_cast<int>(std::ceil(std::max({v1.z, v2.z, v3.z}))));
    return {minZ, maxZ};
}

