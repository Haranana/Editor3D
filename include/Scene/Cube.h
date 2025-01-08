#ifndef CUBE_H
#define CUBE_H

#include "Object3D.h"
#include "RenderableObject3D.h"
#include "cmath"

class Cube : public RenderableObject3D{
public:

    Cube(int sideLength = defaultSideLength);

private:
    int sideLength; //measured in pixels
    static const int defaultSideLength = 100;

};

#endif // CUBE_H
