#ifndef LIGHT_H
#define LIGHT_H
#include "Object3D.h"
#include "Rendering/Buffer.h"
#include "memory"
#include "Math/Vectors.h"
#include "Math/Matrices.h"
#include "Rendering/Colors.h"
class Light : public Object3D{
public:
    enum class LightType{
        DISTANT,
        SPOT,
        POINT
    };

    Color color = Colors::White;
    double bias = 0.001;
    double intensity = 1.0;
    bool castsShadow = true;
    LightType lightType;// = LightType::DISTANT;

};

#endif // LIGHT_H
