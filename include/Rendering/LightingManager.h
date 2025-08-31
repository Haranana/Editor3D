#ifndef LIGHTINGMANAGER_H
#define LIGHTINGMANAGER_H
#include "Math/Vector3.h"
#include "Buffer.h"
#include "Color.h"
#include "Scene/Material.h"
#include "memory"
#include "Scene/Camera.h"

class LightingManager{
public:
    static constexpr double defaultAlbedo = 0.5;
    LightingManager();
    Color shadeColorFR(const Vector3& cameraPosition,
                        const Vector3& point,
                        const Vector3& normal,
                        const Color& color) const;

    double getReflectedLightLambert(Vector3& lightDirection,Vector3& normal, double lightEnergy, double albedo = defaultAlbedo) const;
    Vector3 getReflectedLightLambert(Vector3& lightDirection,Vector3& normal, Vector3 lightEnergy, Vector3 albedo) const;
    Vector3 getDiffuseLambert(Vector3& lightDirection,Vector3& normal, Vector3 lightColor) const;
    Vector3 getDiffuseLambertBRDFMultiplier(const Vector3& pointToLightDir, const Material& material, Camera& camera, const Vector3& worldSpacePoint);

    Vector3 illuminatePointPhong(Vector3& pointToLightDir,Vector3& normal,const Material& material,  Camera& camera, const Vector3& worldSpacePoint,
                                 bool fresnel = false, bool normalizeSpecular = false) ;
    Vector3 illuminatePointBlinnPhong(Vector3& pointToLightDir,Vector3& normal,const Material& material,  Camera& camera, const Vector3& worldSpacePoint,
                                 bool fresnel = false, bool normalizeSpecular = false) ;

    Vector3 getSpecularCookTorrance(Vector3& pointToLightDir,Vector3& normal,const Material& material,  Camera& camera, const Vector3& worldSpacePoint);

    std::shared_ptr<Buffer<double>> shadowMap;
private:
    //returns vector beetwen camera and specified point, used in calculating facing ratio
    double getFacingRatio(const Vector3& cameraPosition,
                          const Vector3& point,
                          const Vector3& normal) const;

    double schlickApproximation(double angleCos, double normIncidenceReflaction = 0.04);
    Vector3 schlickApproximationVector(double angleCos, Vector3 normIncidenceReflaction = Vector3{0.04,0.04,0.04});

    double ndfGgx(double roughness, const Vector3& normal, const Vector3& halfwayVector);
    double schlickGgx(double roughness, const Vector3& normal, const Vector3& pointToCameraDir, const Vector3& pointToLightDir);

    Vector3 getCameraPointVector(const Vector3& cameraPosition,
                                 const Vector3& point) const;
};

#endif // LIGHTINGMANAGER_H
