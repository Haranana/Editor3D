#ifndef TRIANGLE3_H
#define TRIANGLE3_H
#include <algorithm>
#include "Vectors.h"
#include "Math/Triangle.h"

class Triangle3 : public Triangle<Vector3>{
public:

    constexpr Triangle3(const Vector3& v1, const Vector3& v2, const Vector3& v3) : Triangle(v1, v2, v3){}
    bool isInTriangle2D(const Vector3& v); //checks if given vector is inside area of triangle, checks only x and y coords
    bool isInTriangle2D(const Vector2& v);
    double area();
    Triangle<double> baricentricCoords(const Vector3& v);
    Triangle<double> baricentricCoords(const Vector2& v);

    std::pair<int, int> getBorderX(int lowest, int highest);
    std::pair<int, int> getBorderY(int lowest, int highest);
    std::pair<int, int> getBorderZ(int lowest, int highest);
};

#endif // TRIANGLE3_H
