#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QColor>
#include "Rendering/Color.h"
#include "ColorPreview.h"
#include <iostream>



class ColorPicker : public QWidget {
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = nullptr);
    void setColor(const Color& color);
    Color getColor();

private slots:
    void updateColor();
signals:
    void colorChanged(const Color &newColor);

private:
    QLineEdit *rEdit;
    QLineEdit *gEdit;
    QLineEdit *bEdit;
    Color color;
    ColorPreview *colorPreview;
};

#endif // COLORPICKER_H
