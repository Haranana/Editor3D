#ifndef LIGHT_H
#define LIGHT_H
#include "Object3D.h"
#include "Rendering/Buffer.h"
#include "memory"
#include "Math/Vectors.h"
#include "Math/Matrices.h"
#include "Rendering/Colors.h"
class Light : public Object3D{

    Color color = Colors::White;
    double intensity = 1.0;
    bool castsShadow = true;

};

#endif // LIGHT_H
