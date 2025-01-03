#include <Rendering/PixelPainter.h>

PixelPainter::PixelPainter(std::shared_ptr<QImage> img) : img(img){}

PixelPainter::~PixelPainter(){}

void PixelPainter::fillImage(Color color){
    unsigned char *ptr = img->bits();
    int bytesPerLine = img->bytesPerLine();

    for (int y = 0; y < img->height(); y++) {
        for (int x = 0; x < img->width(); x++) {
            int idx = bytesPerLine * y + x * 4;
            ptr[idx] = color.B;
            ptr[idx + 1] = color.G;
            ptr[idx + 2] = color.R;
            ptr[idx + 3] = color.A;
        }
    }
}

void PixelPainter::drawPixel(Vector2 vector , Color color){
    int x = vector.x;
    int y = vector.y;

    if (x >= 0 && x < img->width() && y >= 0 && y < img->height()) {
        unsigned char *ptr = img->bits();
        int bytesPerLine = img->bytesPerLine();
        int idx = bytesPerLine * y + x * 4;
        ptr[idx] = color.B;
        ptr[idx + 1] = color.G;
        ptr[idx + 2] = color.R;
        ptr[idx + 3] = 255;
    }
}

void PixelPainter::drawPixel(Vector2 vector){
    Color color = defaultColor;
    int x = vector.x;
    int y = vector.y;

    if (x >= 0 && x < img->width() && y >= 0 && y < img->height()) {
        unsigned char *ptr = img->bits();
        int bytesPerLine = img->bytesPerLine();
        int idx = bytesPerLine * y + x * 4;
        ptr[idx] = color.B;
        ptr[idx + 1] = color.G;
        ptr[idx + 2] = color.R;
        ptr[idx + 3] = 255;
    }
}

void PixelPainter::copyImg(std::shared_ptr<QImage>source , std::shared_ptr<QImage>dest){
    unsigned char *srcPtr = source->bits();
    unsigned char *destPtr = dest->bits();
    int bytesPerLine = source->bytesPerLine();


    for(int x = 0; x <img->width(); x++){
        for(int y = 0; y<img->height(); y++){
            int baseId = y * bytesPerLine + x * 4;

            destPtr[baseId] = srcPtr[baseId];
            destPtr[baseId + 1] = srcPtr[baseId+1];
            destPtr[baseId + 2] = srcPtr[baseId+2];
            destPtr[baseId + 3] = srcPtr[baseId+3];

        }
    }
}

Color PixelPainter::getColorFromPoint(const Vector2& vector){
    int x = vector.x;
    int y = vector.y;

    if (x >= 0 && x < img->width() && y >= 0 && y < img->height()) {
        unsigned char *ptr = img->bits();
        int bytesPerLine = img->bytesPerLine();
        int idx = bytesPerLine * y + x * 4;

        int B = ptr[idx];
        int G = ptr[idx + 1];
        int R = ptr[idx + 2];
        int A = ptr[idx + 3];

        return Color(B, G, R , A);
        //return Color(R, G, B);
    }
    return Color(0, 0, 0 , 0);
}

void PixelPainter::setImg(std::shared_ptr<QImage>img){
    this->img = img;
}

void PixelPainter::setDefaultColor(Color color){
    defaultColor = color;
}

Color PixelPainter::getDefaultColor(){
    return defaultColor;
}

Color& PixelPainter::getDefaultColorRef(){
    //std::shared_ptr<Color> defaultColorPtr = std::make_shared<Color>(defaultColor);
    return defaultColor;
}
