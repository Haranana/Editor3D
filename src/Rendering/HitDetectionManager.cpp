#include "Rendering/HitDetectionManager.h"

HitDetectionManager::HitDetectionManager(int height, int width){
    idBuffer = std::make_shared<Buffer<IdBufferElement>>(height, width, IdBufferElement());
}

