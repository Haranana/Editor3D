#ifndef HUDOVERLAY_H
#define HUDOVERLAY_H

#include <QImage>
#include <QPainter>
#include <QFontMetrics>

static void drawFpsOverlay(QImage& img,
                           const QString& text,
                           Qt::Corner corner = Qt::TopLeftCorner,
                           int padding = 6,
                           float fontSize = 9.0) {
    QPainter painter(&img);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    QFont font = painter.font();
    font.setPointSizeF(fontSize);
    painter.setFont(font);

    QFontMetrics fm(font);
    const int tw = fm.horizontalAdvance(text);
    const int th = fm.height();

    int x = padding, y = padding;
    if (corner == Qt::TopRightCorner)  x = img.width()  - tw - 2*padding;
    if (corner == Qt::BottomLeftCorner) y = img.height() - th - 2*padding;
    if (corner == Qt::BottomRightCorner){ x = img.width() - tw - 2*padding; y = img.height() - th - 2*padding; }

    QRect box(x, y, tw + 2*padding, th + 2*padding);

    painter.fillRect(box, QColor(0, 0, 0, 140));

    painter.setPen(QColor(0, 0, 0, 200));
    painter.drawText(box.adjusted(1,1,-1,-1), Qt::AlignLeft | Qt::AlignVCenter, text);

    painter.setPen(Qt::white);
    painter.drawText(box, Qt::AlignLeft | Qt::AlignVCenter, text);
}

#endif // HUDOVERLAY_H
