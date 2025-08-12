#include "Scene/ObjectImporter.h"

ImportResult ObjImporter::load(const std::string& objPath, const ImportOptions& opt){

    clearData();
    std::ifstream file(objPath);
    std::string line;

    while(std::getline(file , line)){
        std::string_view line_view = line;
        parseLine(line_view);
    }

    return ImportResult();
}

void ObjImporter::parseLine(std::string_view& line){

    trimComment(line);
    trimEdges(line);
    if(line.empty()) return;

    std::string_view lineType = nextToken(line);

    if(lineType == "v"){

    }else if(lineType =="vt"){

    }else if(lineType == "vn"){

    }else if(lineType == "f"){

    }else if(lineType == "o"){

    }else if(lineType == "mtlib"){

    }else if(lineType == "usemtl"){

    }
}

std::string_view ObjImporter::nextToken(std::string_view& line){

    if(line.empty()) return{};

    auto firstWhiteSpaceId = line.find_first_of(whiteSpaces);
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

    if(parseDouble(xStringView, x) && parseDouble(xStringView, x) && parseDouble(xStringView, x)){
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
        //in future there should be scale and axis conversion
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

    if(parseDouble(xStringView, x) && parseDouble(xStringView, x) && parseDouble(xStringView, x)){
        //in future there should be scale and axis conversion
        vNormals.emplace_back(x,y,z);
    }else{
        //in fututre maybe some warning
    }
}

void ObjImporter::parseF(std::string_view& line){
    if(!startedNewObject){
        startNewObject();
    }



}

void ObjImporter::parseO(std::string_view& line){
    //assuming that object name can have spaces we use everything beside 'o'
    startNewObject(line);

}

void ObjImporter::parseMtlib(std::string_view& line){

}

void ObjImporter::parseUsemtl(std::string_view& line){

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

void ObjImporter::setCurrentObject(const std::string_view& newObjectName){
    auto meshBuilderIterator = meshBuilders.find(newObjectName);
    if(meshBuilderIterator == meshBuilders.end()){
        meshBuilders[newObjectName] = MeshBuilder();
    }

    currentMeshBuilder = &meshBuilders.at(newObjectName);
}

void ObjImporter::clearData(){
    vPositions.clear();
    vTextureUVs.clear();
    vNormals.clear();
    startedNewObject = false;
    meshBuilders.clear();
    currentMeshBuilder = nullptr;
}
