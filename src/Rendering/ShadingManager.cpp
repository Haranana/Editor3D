#include "Rendering/ShadingManager.h"
#include "Math/Utility.h"

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

Vector3 ShadingManager::illuminatePointPhong(Vector3& pointToLightDir,Vector3& normal,const Material& material,  Camera& camera,const Vector3& worldSpacePoint,
                                             bool fresnel , bool normalizeSpecular ) {

    double nDotL = normal.dotProduct(pointToLightDir);
    if(nDotL <= 0) return Vector3();
    nDotL = std::clamp(nDotL , 0.0, 1.0);

    Vector3 specular = material.Ks;
    Vector3 perfectRefDir = ( (normal*(2*(nDotL)))-pointToLightDir).normalize();
    Vector3 pointToCameraDir = (camera.transform.getPosition() - worldSpacePoint).normalize();
    double shininess = material.Ns;

    double RdotV = std::clamp( perfectRefDir.dotProduct(pointToCameraDir) , 0.0 , 1.0);
    Vector3 result= specular*( std::pow(RdotV , shininess));


    if(fresnel){

        double angleCos = RdotV;
        result.x = result.x * schlickApproximation(angleCos);
        result.y = result.y * schlickApproximation(angleCos);
        result.z = result.z * schlickApproximation(angleCos);
    }

    if(normalizeSpecular){
        double normalizeSpecularConst = (shininess+2.0)/(M_PI*2.0);
        result=result*normalizeSpecularConst;
    }

    return result;
}

Vector3 ShadingManager::illuminatePointBlinnPhong(Vector3& pointToLightDir,Vector3& normal,const Material& material,  Camera& camera, const Vector3& worldSpacePoint,
                                  bool fresnel, bool normalizeSpecular){

    double nDotL = normal.dotProduct(pointToLightDir);
    if(nDotL <= 0) return Vector3();

    Vector3 pointToCameraDir = (camera.transform.getPosition() - worldSpacePoint).normalize();
    Vector3 halfwayVector = (pointToLightDir + pointToCameraDir).normalize();

    double nDotHalfway = normal.dotProduct(halfwayVector);
    nDotHalfway = std::max(0.0 , nDotHalfway);

    Vector3 specular = material.Ks;
    double shininess = material.Ns;
    double blinnPhongShininess = 4*shininess;

    Vector3 result= specular*( std::pow(nDotHalfway , blinnPhongShininess));


    double angleCos = pointToCameraDir.dotProduct(halfwayVector);
    angleCos = std::clamp(angleCos, 0.0, 1.0);
    if(fresnel){

        result.x = result.x * schlickApproximation(angleCos);
        result.y = result.y * schlickApproximation(angleCos);
        result.z = result.z * schlickApproximation(angleCos);
    }

    if(normalizeSpecular){
        const double normalizeSpecularConst = (blinnPhongShininess + 8.0) / (8.0 * M_PI);
        result=result*normalizeSpecularConst;
    }

    return result;
}

double ShadingManager::schlickApproximation(double angleCos, double normIncidenceReflaction){
    if(MathUt::equal(normIncidenceReflaction , 0.0)) normIncidenceReflaction = 0.04;
    return normIncidenceReflaction + (1-normIncidenceReflaction)*std::pow((1-angleCos),5);
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
