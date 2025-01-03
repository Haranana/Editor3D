#ifndef PIXELPAINTER_H
#define PIXELPAINTER_H

#include <QImage>
#include <memory>
#include "Color.h"
#include "Math/Vector2.h"

class PixelPainter{
public:

    PixelPainter(std::shared_ptr<QImage> img);
    virtual ~PixelPainter();

    void fillImage(Color color);
    void drawPixel(Vector2 vector , Color color);
    void drawPixel(Vector2 vector);
    void copyImg(std::shared_ptr<QImage>source , std::shared_ptr<QImage>dest);

    Color getColorFromPoint(const Vector2& vector);

    virtual void setImg(std::shared_ptr<QImage>img);
    virtual void setDefaultColor(Color color);
    virtual Color getDefaultColor();
    virtual Color& getDefaultColorRef();

protected:

    std::shared_ptr<QImage>img;
    Color defaultColor = Color(255,255,255);
};
#endif // PIXELPAINTER_H
