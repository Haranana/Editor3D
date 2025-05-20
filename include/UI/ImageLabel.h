#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QImage>
class ImageLabel : public QLabel {
    Q_OBJECT
public:
    explicit ImageLabel(QWidget* parent = nullptr);
    void setImage(const QImage& img);

protected:
    void mousePressEvent(QMouseEvent* ev) override;

private:
    QImage image;

signals:
    void pixelClicked(int x, int y);
};

#endif // IMAGELABEL_H
