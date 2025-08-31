#include "Rendering/LightingManager.h"
#include "Math/Utility.h"

LightingManager::LightingManager() {}

double LightingManager::getReflectedLightLambert(Vector3& lightDirection,Vector3& normal, double lightEnergy, double albedo) const{
    double NdotL = std::max(0.0, normal.dotProduct(lightDirection*(-1.0)));
    return NdotL * albedo * lightEnergy;
}

Vector3 LightingManager::getReflectedLightLambert(Vector3& lightDirection,Vector3& normal, Vector3 lightEnergy, Vector3 albedo) const{
    double NdotL = std::max(0.0, normal.dotProduct(lightDirection));
    return {NdotL * albedo.x * lightEnergy.x,
            NdotL * albedo.y * lightEnergy.y,
            NdotL * albedo.z * lightEnergy.z,
            } ;
}

Vector3 LightingManager::getDiffuseLambert(Vector3& lightDirection,Vector3& normal, Vector3 lightColor) const{
    double NdotL = std::max(0.0, normal.dotProduct(lightDirection));
    return {NdotL * lightColor.x,
            NdotL * lightColor.y,
            NdotL * lightColor.z,
            } ;
}

Vector3 LightingManager::getDiffuseLambertBRDFMultiplier(const Vector3& pointToLightDir, const Material& material, Camera& camera, const Vector3& worldSpacePoint){
    Vector3 pointToCameraDir = (camera.transform.getPosition() - worldSpacePoint).normalize();
    Vector3 halfwayVector = (pointToLightDir + pointToCameraDir).normalize();

    double fresnelAngle = pointToCameraDir.dotProduct(halfwayVector);

    Vector3 F0 = material.Kd * material.metallic
                 + Vector3(0.04,0.04,0.04) * (1.0 - material.metallic);

    Vector3 fresnel = schlickApproximationVector(fresnelAngle, F0);
    Vector3 oneMinusFresnel = Vector3{1.0 - fresnel.x,
                                      1.0 - fresnel.y,
                                        1.0 - fresnel.z};
    return (oneMinusFresnel)*(1.0 - material.metallic)*(1.0/M_PI);
}

Vector3 LightingManager::illuminatePointPhong(Vector3& pointToLightDir,Vector3& normal,const Material& material, Camera& camera,const Vector3& worldSpacePoint,
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

Vector3 LightingManager::illuminatePointBlinnPhong(Vector3& pointToLightDir,Vector3& normal,const Material& material,  Camera& camera, const Vector3& worldSpacePoint,
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


Vector3 LightingManager::getSpecularCookTorrance(Vector3& pointToLightDir,Vector3& normal,const Material& material,  Camera& camera, const Vector3& worldSpacePoint){

    Vector3 pointToCameraDir = (camera.transform.getPosition() - worldSpacePoint).normalize();
    Vector3 halfwayVector = (pointToLightDir + pointToCameraDir).normalize();
    double fresnelAngle = halfwayVector.dotProduct(pointToCameraDir);
    Vector3 incidenceReflection = material.Kd * material.metallic
                 + Vector3(0.04,0.04,0.04) * (1.0 - material.metallic);

    Vector3 fresnel = schlickApproximationVector(fresnelAngle , incidenceReflection); // 0.04 incidence reflection by default

    double nDotV = normal.dotProduct(pointToCameraDir);
    double nDotL = normal.dotProduct(pointToLightDir);
    nDotV = std::clamp(nDotV , 0.0 , 1.0);
    nDotL = std::clamp(nDotL , 0.0 , 1.0);
    if(nDotV == 0 || nDotL ==0) return Vector3{};

    double denom = MathUt::safeDenom(4*nDotV*nDotL);
    double ndf = ndfGgx(material.roughness, normal, halfwayVector);
    double geometry = schlickGgx(material.roughness, normal, pointToCameraDir, pointToLightDir);

    return fresnel*ndf*geometry/denom;

}

Vector3 LightingManager::getConstantAmbient(const Vector3& baseColor, const Vector3& Ka, const Vector3& ambientColor, double ambientIntensity ,
                                            bool ambientPBR , double metallic){

    Vector3 La = ambientColor * ambientIntensity;
    Vector3 ambient = baseColor.hadamard(Ka).hadamard(La);

    if (ambientPBR) {
        ambient = ambient * (1.0 - metallic);
    }

    return ambient;
}

double LightingManager::schlickApproximation(double angleCos, double normIncidenceReflaction){
    if(MathUt::equal(normIncidenceReflaction , 0.0)) normIncidenceReflaction = 0.04;
    return normIncidenceReflaction + (1-normIncidenceReflaction)*std::pow((1-angleCos),5);
}

Vector3 LightingManager::schlickApproximationVector(double angleCos, Vector3 normIncidenceReflaction ){
    angleCos = std::clamp(angleCos, 0.0 , 1.0);
    return normIncidenceReflaction + (Vector3(1,1,1) - normIncidenceReflaction)*std::pow((1-angleCos),5);
}

double LightingManager::ndfGgx(double roughness, const Vector3& normal, const Vector3& halfwayVector){
    double alpha = roughness*roughness;
    double alphaSquared = alpha*alpha;
    double nDotH = normal.dotProduct(halfwayVector);
    nDotH = std::clamp(nDotH, 0.0, 1.0);
    double nDotHSquared = nDotH*nDotH;
    double alphaSquaredMinusOne = alpha*alpha-1;
    double ndfDenomMain = nDotHSquared * alphaSquaredMinusOne + 1;
    nDotHSquared = std::clamp(nDotHSquared , 0.0, 1.0);
    double denom = MathUt::safeDenom(M_PI*(ndfDenomMain*ndfDenomMain) , 1e-12);
    return alphaSquared/denom;
}

double LightingManager::schlickGgx(double roughness, const Vector3& normal, const Vector3& pointToCameraDir, const Vector3& pointToLightDir){
    double k = (roughness+1.0)*(roughness+1.0)/8.0;
    double nDotV = normal.dotProduct(pointToCameraDir);
    double nDotL = normal.dotProduct(pointToLightDir);
    nDotV = std::clamp(nDotV , 0.0 , 1.0);
    nDotL = std::clamp(nDotL , 0.0 , 1.0);
    double denomV = MathUt::safeDenom((nDotV*(1-k)+k) , 1e-12);
    double denomL = MathUt::safeDenom((nDotL*(1-k)+k) , 1e-12);
    return (nDotV/denomV)*(nDotL/denomL);
}


Color LightingManager::shadeColorFR(const Vector3& cameraPosition,
                   const Vector3& point,
                   const Vector3& normal,
                                   const Color& color) const{
    double facingRatio = getFacingRatio(cameraPosition, point, normal);
    return Color(color.B*facingRatio, color.G*facingRatio, color.R*facingRatio);
}

double LightingManager::getFacingRatio(const Vector3& cameraPosition,
                      const Vector3& point,
                          const Vector3& normal) const{
    return std::max(0.0, normal.dotProduct(getCameraPointVector(cameraPosition,point)));
}


Vector3 LightingManager::getCameraPointVector(const Vector3& cameraPosition,
                             const Vector3& point) const{
    return (point - cameraPosition).normalize();
}
