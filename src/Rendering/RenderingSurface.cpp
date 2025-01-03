#include "Rendering/RenderingSurface.h"

RenderingSurface::RenderingSurface(std::shared_ptr<QImage> img) : img(img) {}

void RenderingSurface::clearImg(){
    PixelPainter pixelPainter = PixelPainter(img);
    pixelPainter.fillImage(Colors::Black);
}

std::shared_ptr<QImage> RenderingSurface::getImg() {
    return img;
}

Vector2 RenderingSurface::getMiddle(){
    return Vector2(img->width()/2 , img->height()/2);
}
