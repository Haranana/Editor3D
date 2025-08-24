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

    enum class PcfType{
        NONE,
        PCF_3x3,
        PCF_5x5,
        PCSS
    };

    enum class BiasType{
        CONSTANT,
        SLOPE_SCALED,
        NORMAL_ANGLE
    };

    BiasType biasType = BiasType::CONSTANT;
    PcfType pcfType = PcfType::NONE;
    bool bilinearFiltering = false;

    Color color = Colors::White;
    double bias = 0.03;
    double intensity = 1.0;
    bool castsShadow = true;
    LightType lightType;// = LightType::DISTANT;
    double near = 0.1;

};

#endif // LIGHT_H
