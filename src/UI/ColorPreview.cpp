#include "include/UI/ColorPreview.h"

ColorPreview::ColorPreview(QWidget *parent) : QWidget(parent) {
    m_color = Qt::black;
}

void ColorPreview::setColor(const QColor &color) {
    m_color = color;
    update();
}

void ColorPreview::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setBrush(m_color);
    painter.drawRect(rect());
}
