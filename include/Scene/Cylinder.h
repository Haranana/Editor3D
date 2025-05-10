#ifndef CYLINDER_H
#define CYLINDER_H

#include "Object3D.h"
#include "RenderableObject3D.h"
#include "cmath"

class Cylinder : public RenderableObject3D{
public:

    Cylinder(int radious = defaultRadious, int height = defaultHeight , int verticesInCircle = defaultVerticesInCircle);

private:
    int height; //measured in pixels
    int radious;
    int verticesInCircle;
    static const int defaultHeight = 100;
    static const int defaultRadious = 40;
    static const int defaultVerticesInCircle = 10;

};


#endif // CYLINDER_H
