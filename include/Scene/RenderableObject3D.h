#ifndef RENDERABLEOBJECT3D_H
#define RENDERABLEOBJECT3D_H

#include "Object3D.h"
#include "Rendering/Texture.h"
#include "Rendering/DisplaySettings.h"
#include "Scene/Material.h"
#include "Math/Vector2.h"
#include <QString>

class BasicRenderStrategy;
class RenderStrategy;

class RenderableObject3D : public  Object3D{
public:

    RenderableObject3D();
    RenderableObject3D(const RenderableObject3D& otherObject);

    /*
    std::vector<std::pair<int , int>> edges;
    std::vector<std::vector<int>> faces;
    */

    std::vector<Vector3> vertices;

    //kolejne indeksy wierzcholkow wszystkich scian obiektu, zakladamy, ze kazdy polygon ma dokladnie 3 wierzcholki
    //przykladowo faceVertexIndices[faceId] zwroci id pierwszego wierzcholka w scianie o danej faceId
    std::vector<int> faceVertexIndices;

    //tablica wektorow normalnych ścian
    std::vector<Vector3> normals;

    std::vector<bool> vertexHasNormal;
    std::vector<bool> vertexHasUV;
    std::vector<bool> faceHasUV;

    // dwuwymiarowa lista przechowujaca informacje dla wierzcholka o danym indeksie
    // normalne scian o jakich indeksach z nim granicza
    std::vector<std::vector<int>> vertexToFaceNormals;

    //normalne wierzcholkow;
    //Vector3{0,0,0} is considered placeholder vector indicating lack of calculated normal for i vertex
    std::vector<Vector3> vertexNormals;

    Material material;
    std::shared_ptr<DisplaySettings> displaySettings;

    //wspolrzedne uv tekstur
    //default is Vector2{0,0}
    std::vector<Vector2> textureCoords;
    //tekstura calego obiektu, byc moze w przyszlosci zostanie to zamienione na tekstury scian
    std::shared_ptr<Texture> texture;

    //std::unique_ptr<RenderStrategy> renderStrategy;
    void debugStats(){
        std::cout<<"name: "<<name<<std::endl;
        std::cout<<"vertices: "<<vertices.size()<<std::endl;
        std::cout<<"faceVertexIndices: "<<faceVertexIndices.size()<<std::endl;
        std::cout<<"normals: "<<normals.size()<<std::endl;
        std::cout<<"vertexNormals: "<<vertexNormals.size()<<std::endl;
        std::cout<<"textureCoords: "<<textureCoords.size()<<std::endl;
        std::cout<<std::endl<<std::endl;
    }

    QString toQString(){
        return QString("name: %1, vertices: %2, faces: %3").arg(QString::fromStdString(name)).arg((int)vertices.size()).arg((int)faceVertexIndices.size()/3);
    }

    //post creation object sanitization and missing data calculations;
    void sanitize();

    void removeDegenerateFaces();
    void fixFaceNormals();
    void fixVertexNormals();

};

#endif // RENDERABLEOBJECT3D_H

//TODO poprawic publicznosc metod i zmiennych
