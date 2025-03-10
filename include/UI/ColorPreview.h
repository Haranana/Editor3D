#ifndef COLORPREVIEW_H
#define COLORPREVIEW_H

#include <QWidget>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QColor>

//Widget - Prostokat zakolorowany na dany kolor, wykorzystywany w widgecie ColorPicker
class ColorPreview : public QWidget {
    Q_OBJECT
public:

    explicit ColorPreview(QWidget *parent = nullptr);

    void setColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *) override;


private:
    QColor m_color;
};

#endif
