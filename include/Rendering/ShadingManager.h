#ifndef SHADINGMANAGER_H
#define SHADINGMANAGER_H
#include "Math/Vectors.h"
#include "Math/Matrices.h"
#include "Buffer.h"
#include "Color.h"
#include "memory"

class ShadingManager{
public:
    static constexpr double defaultAlbedo = 0.18;
    ShadingManager();
    Color shadeColorFR(const Vector3& cameraPosition,
                        const Vector3& point,
                        const Vector3& normal,
                        const Color& color) const;

    double getReflectedLightLambert(Vector3& lightDirection,Vector3& normal, double lightEnergy, double albedo = defaultAlbedo) const;

    std::shared_ptr<Buffer<double>> shadowMap;
private:
    //returns vector beetwen camera and specified point, used in calculating facing ratio
    double getFacingRatio(const Vector3& cameraPosition,
                          const Vector3& point,
                          const Vector3& normal) const;

    Vector3 getCameraPointVector(const Vector3& cameraPosition,
                                 const Vector3& point) const;
};

#endif // SHADINGMANAGER_H
