#include "include/Scene/ObjectSaver.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>

void ObjectSaver::saveObject(std::shared_ptr<RenderableObject3D>object){
    std::ofstream file(generateFileName());
    for(const auto& v: object->vertices){
        file<<std::fixed<<std::setprecision(6)<<"v"<<" "<<v.x<<" "<<v.y<<" "<<v.z<<std::endl;
    }

    for(int it=0; it<object->faceVertexIndices.size(); it+=3){
        file<<std::fixed<<std::setprecision(6)<<"f"<<" "<<object->faceVertexIndices[it]+1<<" "<<object->faceVertexIndices[it+1]+1<<" "<<object->faceVertexIndices[it+2]+1<<std::endl;
    }

    file.close();
}


std::string ObjectSaver::generateFileName(){
    auto now = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return std::string("object"+std::to_string(milliseconds)+".obj");
}
