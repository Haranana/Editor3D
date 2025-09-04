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

    enum class BiasType{
        CONSTANT,
        SLOPE_SCALED,
        NORMAL_ANGLE
    };

    virtual double getAttenuation(const Vector3& lightToPoint = {}) = 0;
    virtual Matrix4 getViewMatrix(size_t index = 0) const = 0;
    virtual Matrix4 getProjectionMatrix() const = 0;
    virtual Buffer<double>& getShadowMap(size_t index = 0) = 0;
    virtual const Buffer<double>& getShadowMap(size_t index = 0) const = 0;
    virtual void setShadowMapSize(size_t newSize)= 0;
    BiasType biasType = BiasType::CONSTANT;
    FilteringType filteringType = FilteringType::NONE;
    Color color = Colors::White;
    LightType lightType;// = LightType::DISTANT;
    Vector3 direction; //should be ignored in case of point light

    int    pcfPoissonSamples      = 12;   // 8/12/16
    bool castsShadow = true;
    double pcfPoissonRadiusTexels = 1.8;  // 1.5–2.5 should be good
    double bias = 0.002;
    double range;
    double intensity = 1.0;
    double near = 0.1;
    double emitterRadiusWorld{};

};

#endif // LIGHT_H
