#ifndef GRID_H
#define GRID_H

#include "Object3D.h"
#include "RenderableObject3D.h"
#include "cmath"


class Grid : public RenderableObject3D{
public:

    enum Orientation{
        HORIZONTAL,
        VERTICAL
    };

    Grid(Grid::Orientation orientation = defaultOrientation, int linesInterval = defaultLinesInterval, int size = defaultSize);

private:


    int linesInterval;
    int size;
    Grid::Orientation orientation;

    //in how many pixels of space beetwen lines
    static const Grid::Orientation defaultOrientation = Grid::Orientation::VERTICAL;
    static const int defaultLinesInterval = 50;
    static const int defaultSize = 500;
};

#endif // GRID_H
