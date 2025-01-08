#include "Scene/Cube.h"
#include "Rendering/BasicRenderStrategy.h"

Cube::Cube(int sideLength) : sideLength(sideLength){

    int radious = sideLength/2;

    vertices = {Vector3(-radious , radious , radious),
                Vector3(radious , radious , radious),
                Vector3(radious , -radious , radious),
                Vector3(-radious , -radious , radious),
                Vector3(-radious , radious , -radious),
                Vector3(radious , radious , -radious),
                Vector3(radious , -radious , -radious),
                Vector3(-radious , -radious , -radious)};

    transformedVertices = vertices;

    edges ={{0,1},
            {0,3},
            {0,4},
            {1,5},
            {1,2},
            {2,3},
            {2,6},
            {3,7},
            {4,5},
            {4,7},
            {5,6},
            {6,7}};

    faces = {{0,1,2,3},
             {3,2,6,7},
             {0,1,5,4},
             {4,5,6,7},
             {1,5,6,2},
             {0,4,7,3}};

    renderStrategy = std::make_unique<BasicRenderStrategy>();

    //transform.setPosition(startingPosition);
    //transform.setPosition(Vector3(0,0,0));
}
