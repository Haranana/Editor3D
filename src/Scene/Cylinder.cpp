#include "Scene/Cylinder.h"
#include "Scene/Cube.h"
#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/FrontOnlyRenderStrategy.h"
#include "Rendering/RasterRenderStrategy.h"

Cylinder::Cylinder(int radious, int height) : height(height), radious(radious){

    const int halfHeight = height/2;

    vertices = {Vector3(-radious , halfHeight , -radious),
                Vector3(radious , halfHeight , -radious),
                Vector3(radious , -halfHeight , -radious),
                Vector3(-radious , -halfHeight , -radious),
                Vector3(-radious , halfHeight , radious),
                Vector3(radious , halfHeight , radious),
                Vector3(radious , -halfHeight , radious),
                Vector3(-radious , -halfHeight , radious)};

    transformedVertices = vertices;

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

    renderStrategy = std::make_unique<RasterRenderStrategy>();

}
