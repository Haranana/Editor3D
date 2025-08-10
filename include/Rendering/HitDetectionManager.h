#ifndef HITDETECTIONMANAGER_H
#define HITDETECTIONMANAGER_H

#include <memory>
#include "Rendering/Buffer.h"

class HitDetectionManager{
public:
    struct IdBufferElement{
        bool isEmpty = true;
        bool mock = false;
        int objectId = -1;
        int faceId = -1;
        int vertexId = -1;
        std::pair<int, int> edgeVertices = {-1,-1};
    };

    HitDetectionManager(int height, int width);
    std::shared_ptr<Buffer<IdBufferElement>>idBuffer;

};

#endif // HITDETECTIONMANAGER_H
