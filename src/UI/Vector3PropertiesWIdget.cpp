#include "UI/Vector3PropertiesWidget.h"
#include <QFormLayout>
#include <QHBoxLayout>
#include <cmath>

Vector3PropertiesWidget::Vector3PropertiesWidget(QWidget* parent, double valueMin, double valueMax, double valueStep, double valueToSliderFactor, int decimals, bool addColorPreview)
    :QWidget(parent), valueMin(valueMin) , valueMax(valueMax), valueStep(valueStep) , valueToSliderFactor(valueToSliderFactor), decimals(decimals) , addColorPreview(addColorPreview){
    auto layout = new QFormLayout(this);

    auto xRow = new QWidget(this);
    auto xLayout = new QHBoxLayout(xRow);
    auto yRow = new QWidget(this);
    auto yLayout = new QHBoxLayout(yRow);
    auto zRow = new QWidget(this);
    auto zLayout = new QHBoxLayout(zRow);

    xSpinBox = new QDoubleSpinBox(this);
    xSpinBox->setRange(valueMin, valueMax);
    xSpinBox->setSingleStep(valueStep);
    ySpinBox = new QDoubleSpinBox(this);
    ySpinBox->setRange(valueMin, valueMax);
    ySpinBox->setSingleStep(valueStep);
    zSpinBox = new QDoubleSpinBox(this);
    zSpinBox->setRange(valueMin, valueMax);
    zSpinBox->setSingleStep(valueStep);
    xSpinBox->setDecimals(decimals);
    ySpinBox->setDecimals(decimals);
    zSpinBox->setDecimals(decimals);

    xSlider = new QSlider(Qt::Horizontal, this);
    xSlider->setRange(static_cast<int>(valueMin*valueToSliderFactor), static_cast<int>(valueMax*valueToSliderFactor));
    ySlider = new QSlider(Qt::Horizontal, this);
    ySlider->setRange(static_cast<int>(valueMin*valueToSliderFactor), static_cast<int>(valueMax*valueToSliderFactor));
    zSlider = new QSlider(Qt::Horizontal, this);
    zSlider->setRange(static_cast<int>(valueMin*valueToSliderFactor), static_cast<int>(valueMax*valueToSliderFactor));

    connect(xSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v*this->valueToSliderFactor);
                if (xSlider->value() != iv)
                    xSlider->setValue(iv);
                onValueChanged();
            });
    connect(xSlider, &QSlider::valueChanged,
            [this](int v) {
                double dv = v/this->valueToSliderFactor;
                if (std::abs(xSpinBox->value() - dv) > 1e-5)
                    xSpinBox->setValue(dv);
            });

    connect(ySpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v*this->valueToSliderFactor);
                if (ySlider->value() != iv)
                    ySlider->setValue(iv);
                onValueChanged();
            });

    connect(ySlider, &QSlider::valueChanged,
            [this](int v) {
                double dv = v/this->valueToSliderFactor;
                if (std::abs(ySpinBox->value() - dv) > 1e-5)
                    ySpinBox->setValue(dv);
            });

    connect(zSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v*this->valueToSliderFactor);
                if (zSlider->value() != iv)
                    zSlider->setValue(iv);
                onValueChanged();
            });
    connect(zSlider, &QSlider::valueChanged,
            [this](int v) {
                double dv = v/this->valueToSliderFactor;
                if (std::abs(zSpinBox->value() - dv) > 1e-5)
                    zSpinBox->setValue(dv);
            });




    xLayout->addWidget(xSpinBox);
    xLayout->addWidget(xSlider);
    yLayout->addWidget(ySpinBox);
    yLayout->addWidget(ySlider);
    zLayout->addWidget(zSpinBox);
    zLayout->addWidget(zSlider);

    xLayout->setContentsMargins(0,0,0,0);
    yLayout->setContentsMargins(0,0,0,0);
    zLayout->setContentsMargins(0,0,0,0);

    layout->addRow("X",xRow);
    layout->addRow("Y",yRow);
    layout->addRow("Z",zRow);


    if(addColorPreview){
        colorPreview = new ColorPreview(this);
        colorPreview->setMinimumSize(50, 50);
        layout->addRow(colorPreview);
    }

}

void Vector3PropertiesWidget::setVector(Vector3& vector){
    this->vector = &vector;

    xSpinBox->blockSignals(true);
    ySpinBox->blockSignals(true);
    zSpinBox->blockSignals(true);
    xSpinBox->setValue(valueToDisplay(vector.x));
    ySpinBox->setValue(valueToDisplay(vector.y));
    zSpinBox->setValue(valueToDisplay(vector.z));
    xSpinBox->blockSignals(false);
    ySpinBox->blockSignals(false);
    zSpinBox->blockSignals(false);

    xSlider->blockSignals(true);
    ySlider->blockSignals(true);
    zSlider->blockSignals(true);
    xSlider->setValue(int(std::round(valueToDisplay(vector.x)*valueToSliderFactor)));
    ySlider->setValue(int(std::round(valueToDisplay(vector.y)*valueToSliderFactor)));
    zSlider->setValue(int(std::round(valueToDisplay(vector.z)*valueToSliderFactor)));
    xSlider->blockSignals(false);
    ySlider->blockSignals(false);
    zSlider->blockSignals(false);


    if(addColorPreview){
        colorPreview->setColor(Vectors::vector3ToQColor(*this->vector));
    }
}

void Vector3PropertiesWidget::setDisplayToValue(std::function<double(double)> displayToValue){
    this->displayToValue = displayToValue;
}

void Vector3PropertiesWidget::setValueToDisplay(std::function<double(double)> valueToDisplay){
    this->valueToDisplay = valueToDisplay;
}

void Vector3PropertiesWidget::onValueChanged(){
    if(!vector) return;
    const double dx = std::clamp(xSpinBox->value(), valueMin, valueMax);
    const double dy = std::clamp(ySpinBox->value(), valueMin, valueMax);
    const double dz = std::clamp(zSpinBox->value(), valueMin, valueMax);
    vector->x = displayToValue(dx);
    vector->y = displayToValue(dy);
    vector->z = displayToValue(dz);
    if(addColorPreview){
        std::cout<<*this->vector<<std::endl;
        colorPreview->setColor(Vectors::vector3ToQColor(*this->vector));
    }
    emit valueChanged();
}


