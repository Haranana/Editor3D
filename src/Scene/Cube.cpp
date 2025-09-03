#include "Scene/Cube.h"
#include <algorithm>

Cube::Cube(int sideLength) :RenderableObject3D(), sideLength(sideLength){

    int radious = sideLength/2;

    vertices = {Vector3(-radious , radious , -radious),
                Vector3(radious , radious , -radious),
                Vector3(radious , -radious , -radious),
                Vector3(-radious , -radious , -radious),
                Vector3(-radious , radious , radious),
                Vector3(radious , radious , radious),
                Vector3(radious , -radious , radious),
                Vector3(-radious , -radious , radious)};

    faceVertexIndices = {0,1,2,
                        2,3,0,
                        3,2,6,
                        6,7,3,
                        4,5,1,
                        1,0,4,
                        7,6,5,
                        5,4,7,
                        1,5,6,
                        6,2,1,
                        4,0,3,
                        3,7,4
    };

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
