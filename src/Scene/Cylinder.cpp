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

    for(size_t face = 0; face < faceVertexIndices.size(); face+=3){
        Vector3 v0 = vertices[faceVertexIndices[face]];
        Vector3 v1 = vertices[faceVertexIndices[face+1]];
        Vector3 v2 = vertices[faceVertexIndices[face+2]];

        normals.push_back(((v1-v0).crossProduct(v2-v0)).normalize()); //vertices are declare clockwise so we multiply by -1 to make normal positive
    }

    displaySettings = std::make_unique<DisplaySettings>();

    textureCoords.resize(vertices.size() , Vector2(0.0,0.0));
    vertexHasNormal.resize(vertices.size() , false);
    vertexHasUV.resize(vertices.size() , false);
    faceHasUV.resize(faceVertexIndices.size()/3, false);

    vertexToFaceNormals.resize(vertices.size() , {});
    for (size_t face = 0; face < faceVertexIndices.size(); face += 3) {
        int idx0 = faceVertexIndices[face    ];
        int idx1 = faceVertexIndices[face + 1];
        int idx2 = faceVertexIndices[face + 2];

        int faceNormalIdx = face / 3;
        vertexToFaceNormals[idx0].push_back(faceNormalIdx);
        vertexToFaceNormals[idx1].push_back(faceNormalIdx);
        vertexToFaceNormals[idx2].push_back(faceNormalIdx);
    }

    for (size_t vi = 0; vi < vertices.size(); ++vi) {
        Vector3 nSum(0, 0, 0);
        for (int ni : vertexToFaceNormals[vi]) {
            nSum = nSum + normals[ni];
        }
        vertexNormals.push_back(nSum.normalize());
    }



}
