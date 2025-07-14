#ifndef RENDERABLEOBJECT3D_H
#define RENDERABLEOBJECT3D_H

#include "Rendering/RenderStrategy.h"
#include "Object3D.h"
#include "Rendering/Texture.h"

class BasicRenderStrategy;
class RenderStrategy;

class RenderableObject3D : public  Object3D{
public:

    RenderableObject3D();
    RenderableObject3D(const RenderableObject3D& otherObject);

    void setRenderStrategy(std::unique_ptr<RenderStrategy> newStrategy);

    /*
    std::vector<std::pair<int , int>> edges;
    std::vector<std::vector<int>> faces;
    */

    std::vector<Vector3> vertices;

    std::vector<Vector3> transformedVertices;

    //kolejne indeksy wierzcholkow wszystkich scian obiektu, zakladamy, ze kazdy polygon ma dokladnie 3 wierzcholki
    std::vector<int> faceVertexIndices;

    //tablica wektorow normalnych
    std::vector<Vector3> normals;

    //wspolrzedne uv tekstur
    std::vector<Vector2> textureCoords;
    //tekstura calego obiektu, byc moze w przyszlosci zostanie to zamienione na tekstury scian
    std::shared_ptr<Texture> texture;

    std::unique_ptr<RenderStrategy> renderStrategy;

};

#endif // RENDERABLEOBJECT3D_H

//TODO poprawic publicznosc metod i zmiennych
