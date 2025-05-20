#include "Rendering/HitDetectionManager.h"

HitDetectionManager::HitDetectionManager(int height, int width){
    idBuffer = std::make_shared<std::vector<std::vector<IdBufferElement>>>();
    for(int itY = 0; itY < static_cast<int>(height) ; itY++){
        idBuffer->push_back({});
        for(int itX = 0; itX < static_cast<int>(width) ; itX++){
            (*idBuffer)[itY].push_back(IdBufferElement());
        }
    }
}

void HitDetectionManager::resetIdBuffer(){
    for(int itY = 0; itY < static_cast<int>(idBuffer->size()) ; itY++){
        for(int itX = 0; itX < static_cast<int>((*idBuffer)[itY].size()) ; itX++){
            (*idBuffer)[itY][itX] = IdBufferElement();
        }
    }
}

HitDetectionManager::IdBufferElement HitDetectionManager::getBufferElement(int y, int x){
    return (*idBuffer)[y][x];
}
