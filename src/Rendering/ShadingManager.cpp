#include "Rendering/ShadingManager.h"

ShadingManager::ShadingManager() {}

double ShadingManager::getReflectedLightLambert(Vector3& lightDirection,Vector3& normal, double lightEnergy, double albedo) const{
    double NdotL = std::max(0.0, normal.dotProduct(lightDirection*(-1.0)));
    return NdotL * albedo * lightEnergy;
}

Vector3 ShadingManager::getReflectedLightLambert(Vector3& lightDirection,Vector3& normal, Vector3 lightEnergy, Vector3 albedo) const{
    double NdotL = std::max(0.0, normal.dotProduct(lightDirection));
    return {NdotL * albedo.x * lightEnergy.x,
            NdotL * albedo.y * lightEnergy.y,
            NdotL * albedo.z * lightEnergy.z,
            } ;
}

Color ShadingManager::shadeColorFR(const Vector3& cameraPosition,
                   const Vector3& point,
                   const Vector3& normal,
                                   const Color& color) const{
    double facingRatio = getFacingRatio(cameraPosition, point, normal);
    return Color(color.B*facingRatio, color.G*facingRatio, color.R*facingRatio);
}

double ShadingManager::getFacingRatio(const Vector3& cameraPosition,
                      const Vector3& point,
                          const Vector3& normal) const{
    return std::max(0.0, normal.dotProduct(getCameraPointVector(cameraPosition,point)));
}


Vector3 ShadingManager::getCameraPointVector(const Vector3& cameraPosition,
                             const Vector3& point) const{
    return (point - cameraPosition).normalize();
}
