#include "UI/ImageLabel.h"

ImageLabel::ImageLabel(QWidget* parent): QLabel(parent)
{
    setMouseTracking(true);
}

void ImageLabel::setImage(const QImage& img) {
    image = img;
    setPixmap(QPixmap::fromImage(image));
    setFixedSize(image.size());
}

void ImageLabel::mousePressEvent(QMouseEvent* ev){
    QPoint p = ev->pos();
    if (!image.isNull() &&
        QRect(QPoint(0,0), image.size()).contains(p))
    {
        emit pixelClicked(p.x(), p.y());
    }
    QLabel::mousePressEvent(ev);
}


