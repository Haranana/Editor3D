#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include "Math/Vector3.h"
#include "Rendering/Texture.h"

class Material{
public:
    std::string name;

    //vector parameters
    Vector3 Ka{0,0,0}; //ambient, "minimal light"
    Vector3 Kd{1,1,1}; //diffuse/albedo used in Lambert
    Vector3 Ks{1,1,1}; //specular used in Blinn/Phong
    Vector3 Ke{0,0,0}; //emissive (object own light)

    // parameters
    double Ns = 32.f; //shininess in Blinn/Phong
    double Ni = 1.f; //Index of Refraction used in water/glass etc
    double d  = 1.f; //opacity
    int   illum = 2; //light model preset
    double roughness = 0.5;//used in Cook-Torrence, should never be 0, maybe min = 0.045
    double metallic = 0.0;//used in Cook-Torrence

    //maps
    std::string map_Kd;   // albedo
    std::string map_Ks;   // specular
    std::string map_d;    // opacity (opcjonalnie do cutout)
    std::string map_Ke;   // emissive

    std::shared_ptr<Texture> albedoTexture;
    std::shared_ptr<Texture> specularTexture;
    std::shared_ptr<Texture> opacityTexture;
    std::shared_ptr<Texture> emissiveTexture;

    static constexpr double defaultNs = 32.0;
    static constexpr double defaultNi = 1.0;
    static constexpr double defaultD = 1.0;
    static constexpr int defaultIllum = 2;

    void loadTexturesFromPaths(const QString& sourceObjPath){
        QString pathKd = Texture::resolveMapPath(sourceObjPath, this->map_Kd);
        if (!pathKd.isEmpty()){
            albedoTexture = Texture::loadTextureCached(pathKd);
            if (albedoTexture && !albedoTexture->isLinear()) albedoTexture->toLinear();
        }

        QString pathKs = Texture::resolveMapPath(sourceObjPath, this->map_Ks);
        if (!pathKs.isEmpty()){
            specularTexture = Texture::loadTextureCached(pathKs);
            if (specularTexture && !specularTexture->isLinear()) specularTexture->toLinear();
        }

        QString pathD = Texture::resolveMapPath(sourceObjPath, this->map_d);
        if (!pathD.isEmpty()){
            opacityTexture = Texture::loadTextureCached(pathD);
        }

        QString pathKe = Texture::resolveMapPath(sourceObjPath, this->map_Ke);
        if (!pathKe.isEmpty()){
            emissiveTexture = Texture::loadTextureCached(pathKe);
            if (emissiveTexture && !emissiveTexture->isLinear()) emissiveTexture->toLinear();
        }
    }
};

#endif // MATERIAL_H
