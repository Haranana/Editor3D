#include "include/Scene/ObjectLoader.h"
#include <sstream>
#include <fstream>



std::vector<std::shared_ptr<RenderableObject3D>> ObjectLoader::loadObject(std::string filePath){

    newObjects.clear();
    vertices.clear();
    faces.clear();


    parseFile(filePath);
    createObjects();

    return newObjects;
}


void ObjectLoader::parseFile(std::string filePath){
    std::ifstream file(filePath);
    std::string line;

    while(std::getline(file , line)){
        std::istringstream stream(line);
        std::string type;

        stream >> type;

        if(type == "v"){
            Vector3 vertex;
            stream >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }else if(type == "f"){
            Face face;
            stream >> face.firstVertex >> face.secondVertex >> face.thirdVertex;
            faces.push_back(face);
        }
    }
    file.close();

}

void ObjectLoader::createObjects(){

    std::shared_ptr<RenderableObject3D> object = std::make_shared<RenderableObject3D>();

    object->vertices = vertices;

    for(const auto& face : faces){
        object->faceVertexIndices.push_back(face.firstVertex-1);
        object->faceVertexIndices.push_back(face.secondVertex-1);
        object->faceVertexIndices.push_back(face.thirdVertex-1);

    }




    newObjects.push_back(object);
}

