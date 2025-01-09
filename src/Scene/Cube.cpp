#include "Scene/Cube.h"
#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/FrontOnlyRenderStrategy.h"

Cube::Cube(int sideLength) : sideLength(sideLength){

    int radious = sideLength/2;

    /* przy zalozeniu ze rosnie blizej kamery
    vertices = {Vector3(-radious , radious , radious),
                Vector3(radious , radious , radious),
                Vector3(radious , -radious , radious),
                Vector3(-radious , -radious , radious),
                Vector3(-radious , radious , -radious),
                Vector3(radious , radious , -radious),
                Vector3(radious , -radious , -radious),
                Vector3(-radious , -radious , -radious)};
    */

    //domyslnie szescian jest definiowany w orientacji praworecznej
    //tzn. obiekty o wiekszym parametrze z sa mniejsze, wydaja sie byc oddalone od kamery
    vertices = {Vector3(-radious , radious , -radious),
                Vector3(radious , radious , -radious),
                Vector3(radious , -radious , -radious),
                Vector3(-radious , -radious , -radious),
                Vector3(-radious , radious , radious),
                Vector3(radious , radious , radious),
                Vector3(radious , -radious , radious),
                Vector3(-radious , -radious , radious)};

    transformedVertices = vertices;

    //krawedzie sa zdefiniowane z zalozeniem, ze kolejnosc ich wierzcholkow nie ma znaczenia
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

    /*
    faces = {{0,1,2,3}, // sciana pionowa, z przodu
             {3,2,6,7}, // gorna podstawa
             {0,1,5,4}, // dolna podstawa
             {4,5,6,7}, // sciana pionowa, z tylu
             {1,5,6,2}, // sciana pionowa, z prawej
             {0,4,7,3}}; // sciana pionowa, z lewej
    */

    //wierzcholki sa w kolejnosci counter-clockwise poczynajac od lewego dolnego
    //wierzcholki w scianach z dopiskiem (rev) sa przedstawione w kolejnosci jakie by mialy wedlug powyzszej zasady
    //ale gdyby dana sciana ulegla takiej rotacji aby byc pionowa sciana z przodu
    //(wlasciwie to wszystkie sciany sa tak zapisane ale tylko w niektorych to zmienia kolejnosc)
    faces = {{0,1,2,3}, // sciana pionowa, z przodu
             {3,2,6,7}, // gorna podstawa
             {4,5,1,0}, // dolna podstawa (rev)
             {7,6,5,4}, // sciana pionowa, z tylu (rev)
             {1,5,6,2}, // sciana pionowa, z prawej
             {4,0,3,7}}; // sciana pionowa, z lewej (rev)

    renderStrategy = std::make_unique<FrontOnlyRenderStrategy>();

    //W przyszlosci pewnie dobrze by bylo rozszerzyc program o uzywanie startingPosition
    //transform.setPosition(startingPosition);
    //transform.setPosition(Vector3(0,0,0));
}
