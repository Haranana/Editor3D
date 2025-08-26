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

    enum class FilteringType{
        NONE,
        BILINEAR,
        PCF_3x3,
        PCF_5x5,
        PCF_POISSON,
        PCSS
    };
    FilteringType filteringType = FilteringType::NONE;

    enum class BiasType{
        CONSTANT,
        SLOPE_SCALED,
        NORMAL_ANGLE
    };

    int    pcfPoissonSamples      = 12;   // 8/12/16
    double pcfPoissonRadiusTexels = 1.8;  // 1.5–2.5 should be good

    BiasType biasType = BiasType::CONSTANT;

    Color color = Colors::White;
    double bias = 0.03;
    double intensity = 1.0;
    bool castsShadow = true;
    LightType lightType;// = LightType::DISTANT;
    double near = 0.1;

};

#endif // LIGHT_H
