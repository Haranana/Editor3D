#ifndef CLIPPINGMANAGER_H
#define CLIPPINGMANAGER_H

#include <vector>
#include <map>
#include "Math/Vectors.h"
#include "Colors.h"

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

    /*
     * little helper class for speeding up shading
     * one class for many types of shading, fields which interpolation is not needed should be left empty
     */

    struct ClippedVertex {
        Vector4 clip;         // clip-space (x,y,z,w)
        double  invW;         // 1/w        // used in various interpolations
        Vector3 worldSpaceVertexOverW;     // world-space position/w used in Phong shading
        Vector3 worldSpaceNormalOverW;      // world-space normal/w used in Phong shading
        Vector3 colorOverW; //used in Gauround shading
    };

    std::vector<ClippedVertex> clipTriangle(const std::vector<ClippedVertex>& triangle);
    std::vector<Vector4> clipTriangle(const std::vector<Vector4>& triangle);
    ClippedVertex interpolateClippedVertices(const ClippedVertex& a, const ClippedVertex& b, PlaneType curPlane);
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
