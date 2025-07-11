#ifndef CLIPPINGMANAGER_H
#define CLIPPINGMANAGER_H

#include <vector>
#include <map>
#include "Math/Vector4.h"

/*
 * should simplify clipTriangle to use isVector... and clipLine
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

    //currently returns ((-1,-1,-1,-1),(-1,-1,-1,-1)) when nothing is inside screen, probably should be replaced
    //with something more readable
    std::pair<Vector4, Vector4> clipLine(const std::pair<Vector4, Vector4>& e);
    bool isVectorInsideScreen(const Vector4& v);

private:

    double planeValue(PlaneType planeType, const Vector4& v);
    bool isVectorInsideScreen(PlaneType planeType, const Vector4& v);
    Vector4 intersect(const Vector4& v1, const Vector4& v2, PlaneType planeType);


    std::vector<PlaneType>planes = {LEFT, RIGHT, TOP, BOTTOM, NEAR, FAR};
};

#endif // CLIPPINGMANAGER_H
