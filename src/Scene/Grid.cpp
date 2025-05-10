#include "Scene/Grid.h"
#include "Rendering/BasicRenderStrategy.h"
#include "Rendering/FrontOnlyRenderStrategy.h"
#include "Rendering/RasterRenderStrategy.h"
#include "Rendering/GridRenderStrategy.h"


Grid::Grid(Grid::Orientation orientation, int linesInterval, int size) :orientation(orientation), linesInterval(linesInterval) , size(size){

    const int startPar = -size/2;
    const int endPar = size/2;
    //int squareCounter=0;
    const int verticesPerRow = size/linesInterval+1;

    switch(orientation){
        case Grid::Orientation::HORIZONTAL:
            for(int curZ = startPar; curZ <= endPar; curZ+=linesInterval){
                for(int curX = startPar; curX <= endPar; curX+=linesInterval){
                    vertices.push_back(Vector3(curX , 0.0 , curZ));
                }
            }
            for(int itZ = 0; itZ < verticesPerRow-1; itZ++){
                for(int itX = 0; itX < verticesPerRow-1; itX++){
                    faceVertexIndices.push_back(verticesPerRow*itZ+itX);
                    faceVertexIndices.push_back(verticesPerRow*itZ+itX+1);
                    faceVertexIndices.push_back(verticesPerRow*itZ+itX+verticesPerRow);

                    faceVertexIndices.push_back(verticesPerRow*itZ+itX+verticesPerRow);
                    faceVertexIndices.push_back(verticesPerRow*itZ+itX+verticesPerRow+1);
                    faceVertexIndices.push_back(verticesPerRow*itZ+itX+1);
                }
            }
            break;
        case Grid::Orientation::VERTICAL:
            /*
            vertices.push_back(Vector3(startPar , startPar , 0.0));
            vertices.push_back(Vector3(startPar+linesInterval , startPar , 0.0));
            vertices.push_back(Vector3(startPar , startPar+linesInterval , 0.0));
            vertices.push_back(Vector3(startPar+linesInterval , startPar+linesInterval , 0.0));

            faceVertexIndices.push_back(squareCounter);
            faceVertexIndices.push_back(squareCounter+1);
            faceVertexIndices.push_back(squareCounter+2);

            faceVertexIndices.push_back(squareCounter+2);
            faceVertexIndices.push_back(squareCounter+3);
            faceVertexIndices.push_back(squareCounter+1);
            squareCounter+=4;
            */
            for(int curY = startPar; curY <= endPar; curY+=linesInterval){
                for(int curX = startPar; curX <= endPar; curX+=linesInterval){
                    vertices.push_back(Vector3(curX , curY , 0.0));
                }
            }
            for(int itY = 0; itY < verticesPerRow-1; itY++){
                for(int itX = 0; itX < verticesPerRow-1; itX++){
                    faceVertexIndices.push_back(verticesPerRow*itY+itX);
                    faceVertexIndices.push_back(verticesPerRow*itY+itX+1);
                    faceVertexIndices.push_back(verticesPerRow*itY+itX+verticesPerRow);

                    faceVertexIndices.push_back(verticesPerRow*itY+itX+verticesPerRow);
                    faceVertexIndices.push_back(verticesPerRow*itY+itX+verticesPerRow+1);
                    faceVertexIndices.push_back(verticesPerRow*itY+itX+1);
                }
            }


            break;
    }

    renderStrategy = std::make_unique<GridRenderStrategy>();
    transformedVertices = vertices;


}
