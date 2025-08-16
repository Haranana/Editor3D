#ifndef VECTOR3PROPERTIESWIDGET_H
#define VECTOR3PROPERTIESWIDGET_H

#include "Math/Vector3.h"
#include <QWidget>
#include <QDoubleSpinBox>
#include <QSlider>
#include <functional>

class Vector3PropertiesWidget : public QWidget{
    Q_OBJECT
public:
    Vector3PropertiesWidget(QWidget* parent = nullptr, double valueMin = 0.0, double valueMax = 1.0, double valueStep = 0.01 ,double valueToSliderFactor = 100.0);
    void setVector(Vector3& vector);
    void setDisplayToValue(std::function<double(double)> displayToValue);
    void setValueToDisplay(std::function<double(double)> valueToDisplay);


    const double valueMin;
    const double valueMax;
    const double valueStep;
    const double valueToSliderFactor;
    const int decimals = 2;


private slots:
    void onValueChanged();

private:
    Vector3* vector = nullptr;

    //function used to convert value beetwen one that is displayed and one in memory and reverse
    //eg. on display - angle in degrees, in memory - angle in radians, displayToValue - function that converts degrees to radians
    std::function<double(double)> displayToValue = [](double in){return in;};
    std::function<double(double)> valueToDisplay = [](double in){return in;};

    QDoubleSpinBox* xSpinBox;
    QDoubleSpinBox* ySpinBox;
    QDoubleSpinBox* zSpinBox;
    QSlider* xSlider;
    QSlider* ySlider;
    QSlider* zSlider;


signals:
    void valueChanged();

};

#endif // VECTOR3PROPERTIESWIDGET_H
