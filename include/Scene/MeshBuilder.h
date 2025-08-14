#ifndef MESHBUILDER_H
#define MESHBUILDER_H

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "unordered_map"

// Collection of structs and function that are used in production of Renderable Project from file

struct MeshVertex {
    Vector3 position = {0,0,0};
    Vector2 texcoord = {0,0};
    Vector3 normal   = {0,0,0};
    bool hasUV = false;
    bool hasN  = false;
};

// holds indices of file global v, vt, vn in 0-based standard
// holds -1 if empty
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
    std::vector<int> indices; //similar to faces in RendObj, 0,1,2 -> first face ; 3,4,5 -> second face
    std::unordered_map<MeshTriplet, int, MeshTripletHash> map; //triplet : vertex Index
    int materialId = -1;

    int getOrCreateIndex(const MeshTriplet& triplet,
                              const std::vector<Vector3>& pos,
                              const std::vector<Vector2>& uv,
                              const std::vector<Vector3>& nor)
    {

        if (auto it = map.find(triplet); it != map.end()){
            return it->second;
        }

        MeshVertex meshVertex;
        meshVertex.position = pos[triplet.v];
        if (triplet.vt >= 0){
            meshVertex.texcoord = uv[triplet.vt];
            meshVertex.hasUV = true;
        }
        if (triplet.vn >= 0){
            meshVertex.normal = nor[triplet.vn];
            meshVertex.hasN = true;
        }

        int idx = (int)vertices.size();

        vertices.push_back(meshVertex);
        map.emplace(triplet, idx);

        return idx;
    }
};

#endif // MESHBUILDER_H
