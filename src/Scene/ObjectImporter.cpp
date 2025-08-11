#include "Scene/ObjectImporter.h"

ImportResult ObjImporter::load(const std::string& objPath, const ImportOptions& opt){

    clearData();
    std::ifstream file(objPath);
    std::string line;

    while(std::getline(file , line)){
        parseLine(line);
    }
}

void ObjImporter::parseLine(const std::string_view& line){

}

void ObjImporter::clearData(){
    vPositions.clear();
    vTextureUVs.clear();
    vNormals.clear();
}
