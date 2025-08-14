#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include "Math/Vector3.h"
class Material{
public:
    std::string name;

    //vector parameters
    Vector3 Ka{0,0,0}; //ambient, "minimal light"
    Vector3 Kd{1,1,1}; //diffuse/albedo used in Lambert
    Vector3 Ks{0,0,0}; //specular used in Blinn/Phong
    Vector3 Ke{0,0,0}; //emissive (object own light)

    // parameters
    double Ns = 32.f; //shininess in Blinn/Phong
    double Ni = 1.f; //Index of Refraction used in water/glass etc
    double d  = 1.f; //opacity
    int   illum = 2; //light model preset

    //maps
    std::string map_Kd;   // albedo
    std::string map_Ks;   // specular
    std::string map_d;    // opacity (opcjonalnie do cutout)
    std::string map_Ke;   // emissive

    static constexpr double defaultNs = 32.0;
    static constexpr double defaultNi = 1.0;
    static constexpr double defaultD = 1.0;
    static constexpr int defaultIllum = 2;
};

#endif // MATERIAL_H
