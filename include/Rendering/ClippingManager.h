#ifndef CLIPPINGMANAGER_H
#define CLIPPINGMANAGER_H

#include <vector>
#include <map>
#include "Math/Vector4.h"

/*
 * Unsure whether it's used anywhere
 */
class ClippingManager{
private:
    enum PlaneType{
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        NEAR,
        FAR
    };

public:
    std::vector<Vector4> clipTriangle(const std::vector<Vector4>& triangle);

private:

    double planeValue(PlaneType planeType, const Vector4& v);
    bool isVectorInsideScreen(PlaneType planeType, const Vector4& v);
    Vector4 intersect(const Vector4& v1, const Vector4& v2, PlaneType planeType);


    std::vector<PlaneType>planes = {LEFT, RIGHT, TOP, BOTTOM, NEAR, FAR};
};

#endif // CLIPPINGMANAGER_H
