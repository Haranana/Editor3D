#ifndef LINEPAINTER_H
#define LINEPAINTER_H

#include "PixelPainter.h"

class LinePainter : public PixelPainter{
public:
    using PixelPainter::PixelPainter;

    void drawLine();
    void drawLine(Color color);
    void drawLine(Vector2 lineStart, Vector2 lineEnd);
    void drawLine(Vector2 lineStart, Vector2 lineEnd , Color color);

    void setLineStart(const Vector2& lineStart);
    void setLineEnd(const Vector2& lineEnd);


private:

    Vector2 lineStart;
    Vector2 lineEnd;

    double lineA = 0.0;
    double lineB = 0.0;

    bool iterateThroughX = true;
    bool isVertical = false;
    int iterationDirection = 1;

    void calculateLineInfo();
};

#endif // LINEPAINTER_H
