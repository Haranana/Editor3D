#ifndef VIEWPORTDISPLAY_H
#define VIEWPORTDISPLAY_H

#include "Rendering/Color.h"
#include "Colors.h"

/*
 * Class that contains instructions about ways to render object
 */
class  ViewportDisplay{
public:
    enum DisplayMode{
      WIREFRAME,
      SOLID,
      TEXTURED
    };

    enum SelectMode{
        OBJECTS,
        FACES,
        EDGES,
        VERTICES,
        NONE
    };

    void selectObject(){
        selectedPart == OBJECTS? selectedPart = NONE : selectedPart = OBJECTS;
        selectedVertex = -1;
        selectedEdge = std::make_pair(-1,-1);
        selectedFace = -1;
    }

    void selectFace(int faceId){
        if(selectedPart == FACES){
            selectedPart = NONE;
            selectedVertex = -1;
            selectedEdge = std::make_pair(-1,-1);
            selectedFace = -1;
        }else{
            selectedPart = FACES;
            selectedVertex = -1;
            selectedEdge = std::make_pair(-1,-1);
            selectedFace = faceId;
        }

    }

    void selectEdge(int v1, int v2){
        if(selectedPart == EDGES){
            selectedPart = NONE;
            selectedVertex = -1;
            selectedEdge = std::make_pair(-1,-1);
            selectedFace = -1;
        }else{
            selectedPart = EDGES;
            selectedVertex = -1;
            selectedEdge = std::make_pair(v1,v2);
            selectedFace = -1;
        }
    }

    void selectVertex(int vertexId){
        if(selectedPart == VERTICES && selectedVertex == vertexId){
            selectedPart = NONE;
            selectedVertex = -1;
            selectedEdge = std::make_pair(-1,-1);
            selectedFace = -1;
           }else{
            selectedPart = VERTICES;
            selectedVertex = vertexId;
            selectedEdge = std::make_pair(-1,-1);
            selectedFace = -1;
        }
    }

    void unSelect(){
        selectedPart = NONE;
        selectedVertex = -1;
        selectedEdge = std::make_pair(-1,-1);
        selectedFace = -1;
    }

    DisplayMode displayMode = WIREFRAME;
    Color color = Colors::Gray;
    Color wireframeColor = Color(255,255,255);
    Color selectColor = Colors::Orange;
    SelectMode selectedPart = NONE;

    int selectedVertex = -1;
    std::pair<int,int> selectedEdge = std::make_pair(-1,-1);
    int selectedFace = -1;
};

#endif // VIEWPORTDISPLAY_H
