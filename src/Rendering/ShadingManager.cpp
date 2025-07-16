#include "Rendering/ShadingManager.h"


ShadingManager::ShadingManager() {}

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
