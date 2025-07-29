#ifndef IDBUFFERELEMENT_H
#define IDBUFFERELEMENT_H
#include "algorithm"

struct IdBufferElement{
    bool isEmpty = true;
    bool mock = false;
    int objectId = -1;
    int faceId = -1;
    int vertexId = -1;
    std::pair<int, int> edgeVertices = {-1,-1};
};

#endif // IDBUFFERELEMENT_H
