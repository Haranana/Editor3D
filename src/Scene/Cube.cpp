#include "Scene/Cube.h"

Cube::Cube(int sideLength) : sideLength(sideLength){
    int radious = sideLength/2;

    vertices.push_back(Vector3(radious , radious , radious));

    vertices.push_back(Vector3(-radious , radious , radious));
    vertices.push_back(Vector3(radious , -radious , radious));
    vertices.push_back(Vector3(radious , radious , -radious));

    vertices.push_back(Vector3(-radious , -radious , radious));
    vertices.push_back(Vector3(radious , -radious , -radious));
    vertices.push_back(Vector3(-radious , radious , -radious));

    vertices.push_back(Vector3(-radious , -radious , -radious));

    edges.push_back({ vertices[0], vertices[1] });
    edges.push_back({ vertices[0], vertices[2] });
    edges.push_back({ vertices[0], vertices[3] });

    edges.push_back({ vertices[1], vertices[4] });
    edges.push_back({ vertices[1], vertices[6] });

    edges.push_back({ vertices[2], vertices[4] });
    edges.push_back({ vertices[2], vertices[5] });

    edges.push_back({ vertices[3], vertices[5] });
    edges.push_back({ vertices[3], vertices[6] });

    edges.push_back({ vertices[4], vertices[7] });

    edges.push_back({ vertices[5], vertices[7] });

    edges.push_back({ vertices[6], vertices[7] });

    transform.setPosition(startingPosition);
}
