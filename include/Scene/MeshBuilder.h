#ifndef MESHBUILDER_H
#define MESHBUILDER_H

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "unordered_map"

// Collection of structs and function that are used in production of Renderable Project from file

struct MeshVertex {
    Vector3 position;
    Vector2 texcoord = {0,0};
    Vector3 normal   = {0,0,0};
    bool hasUV = false;
    bool hasN  = false;
};

// klucz deduplikacji: (v,vt,vn) po konwersji na 0-based (−1 gdy brak)
struct MeshTriplet { int v, vt, vn; };

struct MeshTripletHash {
    size_t operator()(const MeshTriplet& t) const {
        return (uint64_t)(t.v + 1) * 0x9E3779B1u
               ^ (uint64_t)(t.vt + 3) * 0x85EBCA77u
               ^ (uint64_t)(t.vn + 7) * 0xC2B2AE3Du;
    }
};

inline bool operator==(const MeshTriplet& a, const MeshTriplet& b){
    return a.v==b.v && a.vt==b.vt && a.vn==b.vn;
}

// builder of single renderableObject3D
struct MeshBuilder {
    std::string name; // preferably from 'o'
    std::string materialName; // active usemtl (if not found then default material)
    std::vector<MeshVertex>  vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<MeshTriplet, uint32_t, MeshTripletHash> map;

    uint32_t getOrCreateIndex(const MeshTriplet& t,
                              const std::vector<Vector3>& pos,
                              const std::vector<Vector2>& uv,
                              const std::vector<Vector3>& nor)
    {
        if (auto it = map.find(t); it != map.end()) return it->second;

        MeshVertex vx{};
        vx.position = pos[t.v];
        if (t.vt >= 0) { vx.texcoord = uv[t.vt]; vx.hasUV = true; }
        if (t.vn >= 0) { vx.normal   = nor[t.vn]; vx.hasN  = true; }
        uint32_t idx = (uint32_t)vertices.size();
        vertices.push_back(vx);
        map.emplace(t, idx);

        return idx;
    }
};

#endif // MESHBUILDER_H
