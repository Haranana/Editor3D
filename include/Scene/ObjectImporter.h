#ifndef OBJECTIMPORTER_H
#define OBJECTIMPORTER_H
#include "Scene/RenderableObject3D.h"
#include "Math/Matrices.h"
#include "Scene/Material.h"
#include <charconv>
#include <string_view>
#include <vector>
#include <sstream>
#include <fstream>
#include <cctype>

struct ImportOptions {

    bool triangulate = true; //fan triangulation
    bool supportNegativeIndices = true;

    //if (0,0) is in left upper corner
    bool flipV = true;

    bool loadMtl = true;

    // engine doesn't support submeshes or similiar technologies
    // importer deals with this in one of 2 possible ways:
    // false - use material specified in first usemtl for whole object
    // true - convert every submesh into new object which uses correct material
    bool splitPerUsemtlAsObjects = true;

    Matrix3 axisConversionMatrix = Matrices3::identity();
    double  globalScale = 1.0;
};

struct ImportResult {
    std::vector<std::shared_ptr<RenderableObject3D>> objects;
    // opcjonalnie: mapa materiałów po nazwie, do diagnostyki
    std::unordered_map<std::string, Material> materialLib;
};

class ObjImporter {
public:
    ImportResult load(const std::string& objPath, const ImportOptions& opt = ImportOptions());
private:
    void parseLine(const std::string_view& line);
    void clearData();

    std::vector<Vector3> vPositions;
    std::vector<Vector2> vTextureUVs;
    std::vector<Vector3> vNormals;
};

#endif // OBJECTIMPORTER_H
