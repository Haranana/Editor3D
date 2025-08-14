#include "Scene/Cylinder.h"

Cylinder::Cylinder(int radious, int height, int verticesInCircle) : height(height), radious(radious), verticesInCircle(verticesInCircle){

    const float halfHeight = height / 2.0f;
    const float angleDelta = 2*M_PI/verticesInCircle;

    for(int it = 0; it < verticesInCircle; it++) {
        float curAngle = it * angleDelta;
        float x = radious * cos(curAngle);
        float z = radious * sin(curAngle);
        vertices.emplace_back(x, -halfHeight, z);
        vertices.emplace_back(x,  halfHeight, z);
    }

    int centerBot = vertices.size();
    vertices.emplace_back(0, -halfHeight, 0);
    int centerTop = vertices.size();
    vertices.emplace_back(0,  halfHeight, 0);

    for(int it = 0; it < verticesInCircle; it++) {
        int next = (it+1)%verticesInCircle;

        faceVertexIndices.push_back(centerBot);
        faceVertexIndices.push_back(next*2);
        faceVertexIndices.push_back(it*2);

        faceVertexIndices.push_back(centerTop);
        faceVertexIndices.push_back(it*2+1);
        faceVertexIndices.push_back(next*2+1);
    }

    for(int it = 0; it < verticesInCircle; it++) {
        int next = (it+1) % verticesInCircle;
        int bot = it*2;
        int top = it*2+1;
        int botN = next*2;
        int topN = next*2+1;

        faceVertexIndices.push_back(bot);
        faceVertexIndices.push_back(botN);
        faceVertexIndices.push_back(topN);

        faceVertexIndices.push_back(topN);
        faceVertexIndices.push_back(top);
        faceVertexIndices.push_back(bot);
    }
    //renderStrategy = std::make_unique<RasterRenderStrategy>();

}
