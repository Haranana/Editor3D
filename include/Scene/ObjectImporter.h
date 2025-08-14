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
#include <algorithm>
#include "MeshBuilder.h"
#include <cctype>
#include "Math/Utility.h"

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
    std::vector<Material> materials;
};

class ObjImporter {
public:
    ImportResult load(const std::string& objPath, const ImportOptions& opt = ImportOptions());
private:


    void parseLine(std::string_view& line);
    std::string_view nextToken(std::string_view& line, const std::string_view& sep = whiteSpaces);

    void trimLeft(std::string_view& line);
    void trimRight(std::string_view& line);
    void trimEdges(std::string_view& line);
    void trimComment(std::string_view& line);

    void parseV(std::string_view& line);
    void parseVt(std::string_view& line);
    void parseVn(std::string_view& line);
    void parseF(std::string_view& line);
    void parseO(std::string_view& line);

    void parseMtllib(std::string_view& line);
    void parseUsemtl(std::string_view& line);

    void parseKa(std::string_view& line);
    void parseKd(std::string_view& line);
    void parseKs(std::string_view& line);
    void parseKe(std::string_view& line);

    void parseNs(std::string_view& line);
    void parseNi(std::string_view& line);
    void parseD(std::string_view& line);
    void parseIllum(std::string_view& line);

    void parseMapKd(std::string_view& line , std::string& currentMtlDir);
    void parseMapKs(std::string_view& line, std::string& currentMtlDir);
    void parseMapD(std::string_view& line, std::string& currentMtlDir);
    void parseMapKe(std::string_view& line, std::string& currentMtlDir);


    void splitMeshTriplet(std::string_view& triplet, std::string_view& v, std::string_view& vt, std::string_view& vn);
    MeshTriplet parseMeshTriplet(std::string_view& v, std::string_view& vt, std::string_view& vn);
    void addFaceFan(const std::vector<MeshTriplet>& meshTriplets);
    std::vector<std::shared_ptr<RenderableObject3D>> meshBuildersToRenderableObjects();

    bool parseInt(std::string_view& token, int& out);
    bool parseDouble(std::string_view& token, double& out);
    bool parseFloat(std::string_view& token, float& out);

    void loadMtlFile(const std::string& mtlFullPath);
    int  getMaterialIdByName(const std::string& name);
    static std::string joinPath(const std::string& base, std::string_view rel);
    bool parse3Floats(std::string_view& line, float& a, float& b, float& c);
    std::string extractMapPathRelativeTo(const std::string& baseDir, std::string_view lineRemainder);



    //changes current object to MeshBuilder with given name, if no is found, then new MeshBuilder is created
    void setCurrentObject(); // default "object"
    void setCurrentObject(const std::string_view& name);
    std::string makeUniqueName(const std::string& base);
    MeshBuilder& startNewBuilderForCurrent();
    void clearData();

    int currentMaterialId = -1;
    std::vector<Material> materials;
    Material* currentMaterial = nullptr;
    std::unordered_map<std::string,int> matNameToId;
    std::string objDir;
    std::string currentMtlDir;
    std::unordered_map<std::string,int> uniqueNameCounter;

    std::vector<Vector3> vPositions;
    std::vector<Vector2> vTextureUVs;
    std::vector<Vector3> vNormals;
    std::map<std::string , MeshBuilder> meshBuilders;
    //used for trimming
    constexpr static std::string_view whiteSpaces = " \t\n\r\f\v";


    MeshBuilder* currentMeshBuilder = nullptr;
    std::string currentObjectBaseName;
    ImportOptions importOptions;

    //whether new MeshBuilder was built,
    //turn to true when calling startNewObject()
    //if startedNewObject = false when parsing face then startNewObject() is to be called
    bool startedNewObject = false;
};

#endif // OBJECTIMPORTER_H
