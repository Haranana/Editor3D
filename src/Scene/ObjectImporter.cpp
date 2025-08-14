#include "Scene/ObjectImporter.h"
#include <filesystem>

ImportResult ObjImporter::load(const std::string& objPath, const ImportOptions& opt){

    clearData();
    importOptions = opt;
    objDir = std::filesystem::path(objPath).parent_path().string();

    std::ifstream file(objPath);
    std::string line;

    while(std::getline(file , line)){
        std::string_view line_view = line;
        parseLine(line_view);
    }

    ImportResult result;

    std::vector<std::shared_ptr<RenderableObject3D>> loadedObjects = meshBuildersToRenderableObjects();
    result.objects = loadedObjects;
    result.materials = materials;

    return result;
}

void ObjImporter::parseLine(std::string_view& line){

    trimComment(line);
    trimEdges(line);
    if(line.empty()) return;

    std::string_view lineType = nextToken(line);

    if(lineType == "v"){
        parseV(line);
    }else if(lineType =="vt"){
        parseVt(line);
    }else if(lineType == "vn"){
        parseVn(line);
    }else if(lineType == "f"){
        parseF(line);
    }else if(lineType == "o"){
        parseO(line);
    }else if(lineType == "mtllib"){
        parseMtllib(line);
    }else if(lineType == "usemtl"){
        parseUsemtl(line);
    }
}

std::string_view ObjImporter::nextToken(std::string_view& line, const std::string_view& sep){

    if(line.empty()) return{};

    auto firstWhiteSpaceId = line.find_first_of(sep);
    std::string_view token = line.substr(0 , firstWhiteSpaceId);

    if (firstWhiteSpaceId == std::string_view::npos){
        line.remove_prefix(line.size());
    }else{
        line.remove_prefix(firstWhiteSpaceId);
        trimLeft(line);

    }
    return token;
}

void ObjImporter::trimLeft(std::string_view& line){
    auto firstNonEmptyIndex = line.find_first_not_of(whiteSpaces);
    if(firstNonEmptyIndex == std::string_view::npos) firstNonEmptyIndex = line.size();
    line.remove_prefix(firstNonEmptyIndex);
}

void ObjImporter::trimRight(std::string_view& line){
    auto firstNonEmptyIndex = line.find_last_not_of(whiteSpaces);
    if(firstNonEmptyIndex == std::string_view::npos){
        line.remove_suffix(line.size());
    }else{
        line.remove_suffix(line.size() - (firstNonEmptyIndex +1));
    }
}

void ObjImporter::trimEdges(std::string_view& line){
    trimRight(line);
    trimLeft(line);
}

void ObjImporter::trimComment(std::string_view& line){
    auto hashIndex = line.find_first_of('#');
    if(hashIndex == std::string_view::npos) return;

    line.remove_suffix(line.size() - hashIndex);
}

void ObjImporter::parseV(std::string_view& line){
    double x,y,z;

    std::string_view xStringView = nextToken(line);
    std::string_view yStringView = nextToken(line);
    std::string_view zStringView = nextToken(line);

    if(xStringView.empty() || yStringView.empty() || zStringView.empty()) return;

    if(parseDouble(xStringView, x) && parseDouble(yStringView, y) && parseDouble(zStringView, z)){
        //in future there should be scale and axis conversion
        vPositions.emplace_back(x,y,z);
    }else{
        //in fututre maybe some warning
    }
}

void ObjImporter::parseVt(std::string_view& line){
    double u,v;

    std::string_view uStringView = nextToken(line);
    std::string_view vStringView = nextToken(line);

    if(uStringView.empty() || vStringView.empty()) return;

    if(parseDouble(uStringView, u) && parseDouble(vStringView, v)){
        if (importOptions.flipV) v = 1.0f - v;
        vTextureUVs.emplace_back(u,v);
    }else{
        //in fututre maybe some warning
    }
}

void ObjImporter::parseVn(std::string_view& line){
    double x,y,z;

    std::string_view xStringView = nextToken(line);
    std::string_view yStringView = nextToken(line);
    std::string_view zStringView = nextToken(line);

    if(xStringView.empty() || yStringView.empty() || zStringView.empty()) return;

    if(parseDouble(xStringView, x) && parseDouble(yStringView, y) && parseDouble(zStringView, z)){
        //in future there should be scale and axis conversion
        vNormals.emplace_back(x,y,z);
    }else{
        //in fututre maybe some warning
    }
}

void ObjImporter::parseF(std::string_view& line){
    if(!currentMeshBuilder){
        setCurrentObject(); //if there wasn't any 'o' before this face we create new object ourselves
        startNewBuilderForCurrent();
    }

    std::vector<MeshTriplet> meshTriplets;
    while(!line.empty()){
        std::string_view polygonVertexStringView = nextToken(line);
        std::string_view v;
        std::string_view vt;
        std::string_view vn;
        splitMeshTriplet(polygonVertexStringView , v , vt, vn);
        MeshTriplet resultTriplet = parseMeshTriplet(v,vt,vn);

        if(resultTriplet.v == -1){
            meshTriplets.clear();
            break;
        }

        meshTriplets.push_back(resultTriplet);
    }

    addFaceFan(meshTriplets);
}

void ObjImporter::parseO(std::string_view& line){
    //assuming that object name can have spaces we use everything beside 'o'
    setCurrentObject(line);
    currentMaterialId = -1;

}

void ObjImporter::parseMtllib(std::string_view& line){
    while(!line.empty()){
        std::string_view fileName = nextToken(line);
        if(fileName.empty()){
            break;
        }
        loadMtlFile(joinPath(objDir , fileName));
    }
}

void ObjImporter::parseUsemtl(std::string_view& line){
    trimEdges(line);
    if(line.empty()) return;
    currentMaterialId = getMaterialIdByName(std::string(line));
    startNewBuilderForCurrent();

}

void ObjImporter::parseKa(std::string_view& line){
    double x=0.0,y=0.0,z=0.0;

    std::string_view xStringView = nextToken(line);
    std::string_view yStringView = nextToken(line);
    std::string_view zStringView = nextToken(line);

    if(xStringView.empty() || yStringView.empty() || zStringView.empty()) return;

    if(parseDouble(xStringView, x) && parseDouble(yStringView, y) && parseDouble(zStringView, z)){
        currentMaterial->Ka = {x,y,z};
    }else{
        currentMaterial->Ka = {0.0,0.0,0.0};
    }
}

void ObjImporter::parseKd(std::string_view& line){
    double x=1.0,y=1.0,z=1.0;

    std::string_view xStringView = nextToken(line);
    std::string_view yStringView = nextToken(line);
    std::string_view zStringView = nextToken(line);

    if(xStringView.empty() || yStringView.empty() || zStringView.empty()) return;

    if(parseDouble(xStringView, x) && parseDouble(yStringView, y) && parseDouble(zStringView, z)){
        currentMaterial->Kd = {x,y,z};
    }else{
        currentMaterial->Kd = {1.0,1.0,1.0};
    }
}

void ObjImporter::parseKs(std::string_view& line ){
    double x=0.0,y=0.0,z=0.0;

    std::string_view xStringView = nextToken(line);
    std::string_view yStringView = nextToken(line);
    std::string_view zStringView = nextToken(line);

    if(xStringView.empty() || yStringView.empty() || zStringView.empty()) return;

    if(parseDouble(xStringView, x) && parseDouble(yStringView, y) && parseDouble(zStringView, z)){
        currentMaterial->Ks = {x,y,z};
    }else{
        currentMaterial->Ks = {0.0,0.0,0.0};
    }
}

void ObjImporter::parseKe(std::string_view& line){
    double x=0.0,y=0.0,z=0.0;

    std::string_view xStringView = nextToken(line);
    std::string_view yStringView = nextToken(line);
    std::string_view zStringView = nextToken(line);

    if(xStringView.empty() || yStringView.empty() || zStringView.empty()) return;

    if(parseDouble(xStringView, x) && parseDouble(yStringView, y) && parseDouble(zStringView, z)){
        currentMaterial->Ke = {x,y,z};
    }else{
        currentMaterial->Ke = {0.0,0.0,0.0};
    }
}

void ObjImporter::parseNs(std::string_view& line){

    double ns;
    std::string_view nsStringView = nextToken(line);

    if(nsStringView.empty()) return;

    if(parseDouble(nsStringView, ns)){
        currentMaterial->Ns = ns;
    }else{
        currentMaterial->Ns = Material::defaultNs;
    }
}

void ObjImporter::parseNi(std::string_view& line){
    double ni;
    std::string_view niStringView = nextToken(line);

    if(niStringView.empty()) return;

    if(parseDouble(niStringView, ni)){
        currentMaterial->Ni = ni;
    }else{
        currentMaterial->Ni = Material::defaultNi;
    }
}

void ObjImporter::parseD(std::string_view& line){
    double d;
    std::string_view dStringView = nextToken(line);

    if(dStringView.empty()) return;

    if(parseDouble(dStringView, d)){
        currentMaterial->d = d;
    }else{
        currentMaterial->d = Material::defaultD;
    }
}


void ObjImporter::parseIllum(std::string_view& line){
    int illum;
    std::string_view illumStringView = nextToken(line);

    if(illumStringView.empty()) return;

    if(parseInt(illumStringView, illum)){
        currentMaterial->illum = illum;
    }else{
        currentMaterial->illum = Material::defaultIllum;
    }
}

void ObjImporter::parseMapKd(std::string_view& line ,std::string& currentMtlDir){
    currentMaterial->map_Kd = extractMapPathRelativeTo(currentMtlDir , line);
}

void ObjImporter::parseMapKs(std::string_view& line ,std::string& currentMtlDir){
    currentMaterial->map_Ks = extractMapPathRelativeTo(currentMtlDir , line);
}

void ObjImporter::parseMapD(std::string_view& line ,std::string& currentMtlDir){
    currentMaterial->map_d = extractMapPathRelativeTo(currentMtlDir , line);
}

void ObjImporter::parseMapKe(std::string_view& line ,std::string& currentMtlDir){
    currentMaterial->map_Ke = extractMapPathRelativeTo(currentMtlDir , line);
}

void ObjImporter::splitMeshTriplet(std::string_view& triplet, std::string_view& v, std::string_view& vt, std::string_view& vn){

    size_t sep1 = triplet.find('/');
    if (sep1 == std::string_view::npos){
        v=triplet;
        vt={};
        vn={};
        return;
    }

    v = triplet.substr(0, sep1);
    size_t sep2= triplet.find('/', sep1+1);
    if (sep2 == std::string_view::npos){
        vt = triplet.substr(sep1+1);
        vn = {};
    } else {
        vt = triplet.substr(sep1+1, sep2-(sep1+1));
        vn = triplet.substr(sep2+1);
    }
}


MeshTriplet ObjImporter::parseMeshTriplet(std::string_view& vStringView, std::string_view& vtStringView, std::string_view& vnStringView){
    int v,vt,vn;
    MeshTriplet result {-1,-1,-1};

    if(!parseInt(vStringView, v) || v==0 ){
        return result;
    }

    v = MathUt::OnetoZeroBased(v , vPositions.size());
    if(v < 0 || v >= (int)vPositions.size()){
        v = -1;
    }
    result.v = v;

    int vtId = -1;
    if(!vtStringView.empty()){
        parseInt(vtStringView ,vt);
        if(vt >= (int)vTextureUVs.size() || vt ==0){
            vtId = -1;
        }else{
            vtId = MathUt::OnetoZeroBased(vt , vTextureUVs.size());
            if(vtId < 0 || vtId >= (int)vTextureUVs.size()){
                vtId = -1;
            }
        }
        result.vt =  vtId;
    }

    int vnId = -1;
    if(!vnStringView.empty()){
        parseInt(vnStringView ,vn);
        if(vn >= (int)vNormals.size() || vn == 0){
            vnId = -1;
        }else{
            vnId = MathUt::OnetoZeroBased(vn , vNormals.size());
            if(vnId < 0 || vnId >= (int)vNormals.size()){
                vnId = -1;
            }
        }
        result.vn =  vnId;
    }

    return result;
}

void ObjImporter::addFaceFan(const std::vector<MeshTriplet>& meshTriplets){
    if (meshTriplets.size() < 3) return;
    const int i0 = currentMeshBuilder->getOrCreateIndex(meshTriplets[0], vPositions, vTextureUVs, vNormals);
    for (size_t i = 1; i + 1 < meshTriplets.size(); i++) {
        int i1 = currentMeshBuilder->getOrCreateIndex(meshTriplets[i  ], vPositions, vTextureUVs, vNormals);
        int i2 = currentMeshBuilder->getOrCreateIndex(meshTriplets[i+1], vPositions, vTextureUVs, vNormals);
        currentMeshBuilder->indices.push_back(i0);
        currentMeshBuilder->indices.push_back(i1);
        currentMeshBuilder->indices.push_back(i2);
    }
}

std::vector<std::shared_ptr<RenderableObject3D>> ObjImporter::meshBuildersToRenderableObjects(){
    std::vector<std::shared_ptr<RenderableObject3D>> loadedObjects;
    loadedObjects.reserve(meshBuilders.size());

    for(const auto& it: meshBuilders){

        //preinit
        std::shared_ptr<RenderableObject3D> obj = std::make_shared<RenderableObject3D>();
        const MeshBuilder& meshBuilder = it.second;

        size_t verticesAmount = meshBuilder.vertices.size();
        size_t indicesAmount = meshBuilder.indices.size();
        size_t facesAmount = indicesAmount/3;

        obj->vertices.resize(verticesAmount);
        obj->textureCoords.resize(verticesAmount);
        obj->vertexNormals.resize(verticesAmount);
        obj->vertexHasNormal.resize(verticesAmount);
        obj->vertexHasUV.resize(verticesAmount);
        std::vector<Vector3> accum(verticesAmount, Vector3{0,0,0});


        //vertices loop
        for(size_t i = 0; i < verticesAmount; i++){

            //copy vertex
            const MeshVertex& meshVertex = meshBuilder.vertices[i];
            obj->vertices[i] = meshVertex.position;

            //copy vertex UV if aplicable
            if(meshVertex.hasUV){
                obj->textureCoords[i] = meshVertex.texcoord;
                obj->vertexHasUV[i] = true;
            }else{
                obj->textureCoords[i] = Vector2(0,0);
                obj->vertexHasUV[i] = false;
            }

            //copy vertex normal if aplicable
            if(meshVertex.hasN){
                obj->vertexNormals[i] = meshVertex.normal;
                obj->vertexHasNormal[i] = true;
            }else{
                obj->vertexNormals[i] = Vector3{0,0,0};
                obj->vertexHasNormal[i] = false;
            }

        }

        //copy vertex indices for each face
        obj->faceVertexIndices.clear();
        obj->faceVertexIndices.reserve(indicesAmount);
        for (auto idx : meshBuilder.indices) {
            obj->faceVertexIndices.push_back((int)idx);
        }

        obj->normals.resize(facesAmount);
        obj->faceHasUV.assign(facesAmount, false);
        obj->vertexToFaceNormals.assign(verticesAmount, {}); // N pustych list

        //faces loop
        for (size_t f = 0; f < facesAmount; f++) {
            const int i0 = obj->faceVertexIndices[3*f + 0];
            const int i1 = obj->faceVertexIndices[3*f + 1];
            const int i2 = obj->faceVertexIndices[3*f + 2];

            const Vector3& p0 = obj->vertices[i0];
            const Vector3& p1 = obj->vertices[i1];
            const Vector3& p2 = obj->vertices[i2];

            //calculate face normals
            const Vector3 e1 = p1 - p0;
            const Vector3 e2 = p2 - p0;
            Vector3 faceNormal = e1.crossProduct(e2); //not normalized normal
            float len2      = faceNormal.dotProduct(faceNormal);

            Vector3 normalizedFaceNormal;
            if (len2 > 1e-20f) {
                normalizedFaceNormal = faceNormal / std::sqrt(len2);
            } else {
                normalizedFaceNormal = Vector3{0,0,1};
                faceNormal = Vector3{0,0,0};
            }

            obj->normals[f] = normalizedFaceNormal;

            //fill vertexToFace normals;
            obj->vertexToFaceNormals[i0].push_back((int)f);
            obj->vertexToFaceNormals[i1].push_back((int)f);
            obj->vertexToFaceNormals[i2].push_back((int)f);

            //used for calculating vertex normals
            if (!obj->vertexHasNormal[i0]) accum[i0] = accum[i0] + faceNormal;
            if (!obj->vertexHasNormal[i1]) accum[i1] = accum[i1] + faceNormal;
            if (!obj->vertexHasNormal[i2]) accum[i2] = accum[i2] + faceNormal;

            //check which faces do not have UV and fill the list
            if(!obj->vertexHasUV[i0] || !obj->vertexHasUV[i1] || !obj->vertexHasUV[i2]){
                obj->faceHasUV[f] = false;
            }else{
                obj->faceHasUV[f] = true;
            }

        }

         //calculate missing vertex normals and normalize each vertex normal (just to be sure)
        for (size_t vi = 0; vi < verticesAmount; ++vi) {
            if(!obj->vertexHasNormal[vi]){
            obj->vertexNormals[vi]= accum[vi].normalize() ;
            }else{
                obj->vertexNormals[vi] = obj->vertexNormals[vi].normalize();
            }
        }

        int curMeshBuilderMatId = meshBuilder.materialId;
        if(curMeshBuilderMatId>=0 && curMeshBuilderMatId<materials.size()){
            obj->material = materials[meshBuilder.materialId];
        }else{
            obj->material = Material();
        }
        obj->name = meshBuilder.name;

        loadedObjects.push_back(obj);
    }

    return loadedObjects;
}

bool ObjImporter::parseInt(std::string_view& token, int& out){
    const char* begin = token.data();
    const char* end   = token.data() + token.size();

    auto [ptr, ec] = std::from_chars(begin, end, out);
    return (ec == std::errc{} && ptr == end);
}

bool ObjImporter::parseDouble(std::string_view& token, double& out){
    const char* begin = token.data();
    const char* end   = token.data() + token.size();

    auto [ptr, ec] = std::from_chars(begin, end, out);
    return (ec == std::errc{} && ptr == end);
}

bool ObjImporter::parseFloat(std::string_view& token, float& out){
    const char* begin = token.data();
    const char* end   = token.data() + token.size();

    auto [ptr, ec] = std::from_chars(begin, end, out);
    return (ec == std::errc{} && ptr == end);
}

void ObjImporter::loadMtlFile(const std::string& mtlFullPath){
    std::ifstream file(mtlFullPath);
    if (!file) return;

    currentMtlDir = std::filesystem::path(mtlFullPath).parent_path().string();

    currentMaterial = nullptr;
    std::string lineString;

    while (std::getline(file, lineString)){

        std::string_view line = lineString;
        trimComment(line);
        trimEdges(line);
        if (line.empty()) continue;

        std::string_view token = nextToken(line);

        if (token == "newmtl"){
            trimEdges(line);
            if (line.empty()) {
                currentMaterial = nullptr;
                continue;
            }

            int id = getMaterialIdByName(std::string(line));
            currentMaterial = &materials[id];
            currentMaterial->name = std::string(line);

        }
        else if (!currentMaterial){
            continue;
        }
        else if(token == "Ka"){
            parseKa(line);
        }
        else if(token == "Kd"){
            parseKd(line);
        }
        else if(token == "Ks"){
            parseKs(line);
        }
        else if(token == "Ke"){
            parseKe(line);
        }
        else if (token == "Ns"){
            parseNs(line);
        }
        else if (token == "Ni"){
            parseNi(line);
        }
        else if (token == "d"){
            parseD(line);
        }
        else if (token == "illum"){
            parseIllum(line);
        }
        else if (token == "map_Kd"){
            parseMapKd(line, currentMtlDir);
        }
        else if (token == "map_Ks"){
            parseMapKs(line, currentMtlDir);
        }
        else if (token == "map_d"){
            parseMapD(line, currentMtlDir);
        }
        else if (token == "map_Ke"){
            parseMapKe(line, currentMtlDir);
        }
    }
}

int ObjImporter::getMaterialIdByName(const std::string& name){
    auto it = matNameToId.find(name);
    if (it != matNameToId.end()) return it->second;
    Material m; m.name = name;
    int id = (int)materials.size();
    materials.push_back(std::move(m));
    matNameToId.emplace(name, id);
    return id;
}

std::string ObjImporter::joinPath(const std::string& base, std::string_view rel){
    if (rel.empty()) return base;
    std::filesystem::path p = std::filesystem::path(base) / std::filesystem::path(std::string(rel));
    return p.lexically_normal().string();
}

bool ObjImporter::parse3Floats(std::string_view& line, float& a, float& b, float& c){
    std::string_view ta = nextToken(line);
    std::string_view tb = nextToken(line);
    std::string_view tc = nextToken(line);
    return parseFloat(ta, a) && parseFloat(tb, b) && parseFloat(tc, c);
}

std::string ObjImporter::extractMapPathRelativeTo(const std::string& baseDir, std::string_view remainder){
    trimEdges(remainder);
    std::string_view chosen;
    while (!remainder.empty()){
        std::string_view tok = nextToken(remainder);
        if (tok.empty()) break;
        if (tok.front() == '-') continue;
        chosen = tok;
    }
    if (chosen.empty()) return {};
    return joinPath(baseDir, chosen);
}

void ObjImporter::setCurrentObject(){
    setCurrentObject("object");
}

void ObjImporter::setCurrentObject(const std::string_view& newObjectName){

    std::string name(newObjectName);

    if(name.empty()) name="object";
    currentObjectBaseName = std::move(name);

    currentMeshBuilder = nullptr;
}

std::string ObjImporter::makeUniqueName(const std::string& base){

    int& uniqueNameCount = uniqueNameCounter[base];
    std::string name = (uniqueNameCount == 0 ? base : base + "." + std::to_string(uniqueNameCount));

    while (meshBuilders.count(name)) {
        uniqueNameCount++;
        name = base + "." + std::to_string(uniqueNameCount);
    }
    uniqueNameCount++;

    return name;
}

MeshBuilder& ObjImporter::startNewBuilderForCurrent(){

    std::string base = currentObjectBaseName.empty() ? "object" : currentObjectBaseName;

    std::string display = base;
    if (currentMaterialId >= 0) {
        const std::string& m = materials[currentMaterialId].name;
        display += " (mtl: " + m + ")";
    }

    std::string key = makeUniqueName(display);

    MeshBuilder fresh;
    fresh.name = key;
    fresh.materialId = currentMaterialId;

    auto [it,_] = meshBuilders.emplace(key, std::move(fresh));
    currentMeshBuilder = &it->second;
    return *currentMeshBuilder;
}

void ObjImporter::clearData(){
    vPositions.clear();
    vTextureUVs.clear();
    vNormals.clear();
    startedNewObject = false;
    meshBuilders.clear();
    currentMeshBuilder = nullptr;
    importOptions = ImportOptions();
    currentMaterialId = -1;
    materials.clear();
    matNameToId.clear();
    objDir.clear();
    currentMtlDir.clear();
    currentObjectBaseName.clear();
    uniqueNameCounter.clear();
    currentMaterial = nullptr;
}

