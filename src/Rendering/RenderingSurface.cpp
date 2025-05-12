#include "Rendering/RenderingSurface.h"

RenderingSurface::RenderingSurface(std::shared_ptr<QImage> img) : img(img) {}

/*
void RenderingSurface::clearImg(){
    //PixelPainter pixelPainter = PixelPainter(img);
    //pixelPainter.fillImage(Colors::Black);
}
*/

std::shared_ptr<QImage> RenderingSurface::getImg() {
    return img;
}

Vector2 RenderingSurface::getMiddle(){
    return Vector2(img->width()/2 , img->height()/2);
}

Vector2 RenderingSurface::ScreenCoordsToImgCoords(Vector2 coords){
    return coords+getMiddle();
}

Vector2 RenderingSurface::imgCoordsToScreenCoords(Vector2 coords){
    return coords-getMiddle();
}

bool RenderingSurface::areScreenCoordsInImg(Vector2 coords){
    Vector2 imgCoords = ScreenCoordsToImgCoords(coords);
    return imgCoords.x >= 0 && imgCoords.y >= 0 && imgCoords.x < img->width() && imgCoords.y < img->height();
}

Vector2 RenderingSurface::getImgBegScreenCoords(){
    return imgCoordsToScreenCoords(Vector2(0,0));
}

Vector2 RenderingSurface::getImgEndScreenCoords(){
    return imgCoordsToScreenCoords(Vector2(img->width(),img->height()));
}
