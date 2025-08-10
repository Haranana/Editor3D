#ifndef UVPROJECTOR_H
#define UVPROJECTOR_H

#include "Scene/RenderableObject3D.h"

enum class Plane { XY, XZ, YZ };

inline void generatePlanarUV(RenderableObject3D& obj, Plane p)
{
    double minA=+1e9, minB=+1e9, maxA=-1e9, maxB=-1e9;

    auto pick = [&](const Vector3& v)->std::pair<double,double>{
        switch(p){
        case Plane::XY: return {v.x, v.y};
        case Plane::XZ: return {v.x, v.z};
        case Plane::YZ: return {v.y, v.z};
        }
        return {0,0};
    };

    // 1) bounding-box
    for(auto& v : obj.vertices){
        auto [a,b] = pick(v);
        minA = std::min(minA,a);  maxA = std::max(maxA,a);
        minB = std::min(minB,b);  maxB = std::max(maxB,b);
    }
    double dA = maxA-minA, dB = maxB-minB;
    if (dA == 0) {dA = 1;};  if (dB == 0){ dB = 1;}


    // 2) UV
    obj.textureCoords.resize(obj.vertices.size());
    for(size_t i=0;i<obj.vertices.size();++i){
        auto [a,b] = pick(obj.vertices[i]);
        obj.textureCoords[i] = Vector2( (a-minA)/dA , (b-minB)/dB );
    }
}

#endif // UVPROJECTOR_H
