#ifndef HITDETECTIONMANAGER_H
#define HITDETECTIONMANAGER_H
#include <memory>
#include <vector>
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

    void resetIdBuffer();
    IdBufferElement getBufferElement(int y, int x);

    std::shared_ptr<std::vector<std::vector<IdBufferElement>>>idBuffer;
private:

};

#endif // HITDETECTIONMANAGER_H
