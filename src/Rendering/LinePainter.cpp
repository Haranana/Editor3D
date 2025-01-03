#include <Rendering/LinePainter.h>

void LinePainter::drawLine(){
    calculateLineInfo();
    if(isVertical && iterationDirection==1){
        for(int y=lineStart.y ; y<=lineEnd.y ; y+=1 ){
            drawPixel( Vector2( lineStart.x , y) , defaultColor);
        }
    }
    else if(isVertical && iterationDirection==-1){
        for(int y=lineStart.y ; y>=lineEnd.y ; y-=1){
            drawPixel( Vector2( lineStart.x , y) , defaultColor);
        }
    }
    else if(iterateThroughX && iterationDirection==1){
        for(int x=lineStart.x ; x<=lineEnd.x ; x+=1 ){
            drawPixel( Vector2(x , lineA*x+lineB) , defaultColor);
        }
    }
    else if(iterateThroughX && iterationDirection==-1){
        for(int x=lineStart.x ; x>=lineEnd.x ; x-=1 ){
            drawPixel( Vector2(x , lineA*x+lineB) , defaultColor);
        }
    }
    else if(!iterateThroughX && iterationDirection==1){
        for(int y=lineStart.y ; y<=lineEnd.y ; y+=1 ){
            drawPixel( Vector2((y-lineB)/lineA , y) , defaultColor);
        }
    }
    else if(!iterateThroughX && iterationDirection==-1)
    {
        for(int y=lineStart.y ; y>=lineEnd.y ; y-=1){
            drawPixel( Vector2((y-lineB)/lineA , y) , defaultColor);
        }
    }
    lineA = 0.0;
    lineB = 0.0;
}

void LinePainter::drawLine(Color color){
    calculateLineInfo();
    if(isVertical && iterationDirection==1){
        for(int y=lineStart.y ; y<=lineEnd.y ; y+=1 ){
            drawPixel( Vector2( lineStart.x , y) , color);
        }
    }
    else if(isVertical && iterationDirection==-1){
        for(int y=lineStart.y ; y>=lineEnd.y ; y-=1){
            drawPixel( Vector2( lineStart.x , y) , color);
        }
    }
    else if(iterateThroughX && iterationDirection==1){
        for(int x=lineStart.x ; x<=lineEnd.x ; x+=1 ){
            drawPixel( Vector2(x , lineA*x+lineB) , color);
        }
    }
    else if(iterateThroughX && iterationDirection==-1){
        for(int x=lineStart.x ; x>=lineEnd.x ; x-=1 ){
            drawPixel( Vector2(x , lineA*x+lineB) , color);
        }
    }
    else if(!iterateThroughX && iterationDirection==1){
        for(int y=lineStart.y ; y<=lineEnd.y ; y+=1 ){
            drawPixel( Vector2((y-lineB)/lineA , y) , color);
        }
    }
    else if(!iterateThroughX && iterationDirection==-1)
    {
        for(int y=lineStart.y ; y>=lineEnd.y ; y-=1){
            drawPixel( Vector2((y-lineB)/lineA , y) , color);
        }
    }
    lineA = 0.0;
    lineB = 0.0;
}

void LinePainter::drawLine(Vector2 lineStart , Vector2 lineEnd, Color color) {

    setLineStart(lineStart);
    setLineEnd(lineEnd);

    calculateLineInfo();

    drawLine(color);
}

void LinePainter::drawLine(Vector2 lineStart , Vector2 lineEnd) {

    setLineStart(lineStart);
    setLineEnd(lineEnd);

    calculateLineInfo();

    drawLine();
}

void LinePainter::calculateLineInfo(){

    if(lineStart.x == lineEnd.x){
        iterateThroughX = false;
        isVertical = true;
        if(lineStart.y <= lineEnd.y) iterationDirection = 1;
        else iterationDirection = -1;
        \
            return;
    }

    isVertical = false;
    lineA = 1.0*(lineStart.y - lineEnd.y)/(lineStart.x - lineEnd.x);
    lineB = lineStart.y - lineA*lineStart.x;

    if(lineA >= -1 && lineA <= 1) iterateThroughX = true;
    else iterateThroughX = false;

    iterationDirection = 1;
    if(iterateThroughX && lineStart.x > lineEnd.x){
        iterationDirection = -1;
    }
    else if(!iterateThroughX && lineStart.y > lineEnd.y){
        iterationDirection = -1;
    }
}

void LinePainter::setLineStart(const Vector2& vector){
    lineStart = vector;
}

void LinePainter::setLineEnd(const Vector2& vector){
    lineEnd = vector;
}

