#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include <qstring.h>
#include "include/Scene/RenderableObject3D.h"
#include <vector>
#include "include/Math/Vector3.h"

/*
 * Klasa odpowiedzialna za wczytywanie obiektow 3D do sceny
 * Klasa jest w stanie wczytywac tylko i wylacznie pliki w formacjie OBJ
 *
 * Wewnatrz formatu OBJ, czytane sa tylko podstawowe wierzcholki i sciany (czyli oznaczenia v i f)
 * W przyszlosci rowniez wczytywanie beda koordynaty tekstur (vt)
 *
 * Ze wzgledu na ograniczenia dostepne jest tylko wczytywanie scian w formacie f v1 v2 v3 ; gdzie v to dodatnie liczby naturalne, nie obsluguje sie ujemnego indeksowania
 * W przyszlosci pewnie bedzie to rozszerzone o koordynaty tekstur
 *
 * Ze wzgledu na zalozenia projketu, na ta chwile oblsugiwane sa tylko polygony - trojkaty
 *
 * Na ta chwile klasa wspiera tylko wczytywanie 1 obiektu, ale w przyszlosci bedzie to rozbudowane o obsluge grup i wielu obiektow
 * Z tego powodu metoda loadObject zwraca vector obiektow, a nie jeden
 */
class ObjectLoader{
public:
    std::vector<std::shared_ptr<RenderableObject3D>> loadObject(std::string filePath);

private:

    void parseFile(std::string filePath);
    void createObjects();

    /*
     * Odczytywane z pliku informacje o obiektach zapisujemy w tymczasowych strukturach,
     * a dopiero potem do rzeczywistych obiektow RenderableObject
     *
     * Ma to na celu uproszczenia ewentualnej refactoryzacji kodu, w razie zmienienia sie logiki RenderableObject
     *
    */
    struct Face {
        int firstVertex = -1;
        int secondVertex = -1;
        int thirdVertex = -1;
    };

    std::vector<std::shared_ptr<RenderableObject3D>> newObjects;

    //inicjalizujemy liste wierzcholkow z poczatkowym parametrem, poniewaz format obj indeksuje wierzcholki od jedynki
    std::vector<Vector3> vertices;

    std::vector<Face> faces;

};

#endif // OBJECTLOADER_H

/*
 * Notki do formatu:
 *
 * Wierzcholki sa indeksowane od jedynki, np: obj: v 1 2 3 edytor: v 0 1 2
 *
 *
 */
