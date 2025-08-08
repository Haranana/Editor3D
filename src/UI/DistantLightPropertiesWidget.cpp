#include "UI/DistantLightPropertiesWidget.h"

DistantLightPropertiesWidget::DistantLightPropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    lightPropertiesWidget = new LightPropertiesWidget();
    layout->addRow(lightPropertiesWidget);

    auto dirXRow = new QWidget(this);
    auto dirXLayout = new QHBoxLayout(dirXRow);
    dirXSpin = new QDoubleSpinBox(this);
    dirXSpin->setRange(dirMin, dirMax);
    dirXSpin->setSingleStep(dirStep);

    dirXSlider = new QSlider(Qt::Horizontal, this);
    dirXSlider->setRange(int(dirMin*100), int(dirMax*100)); // skala *100

    dirXLayout->addWidget(dirXSpin);
    dirXLayout->addWidget(dirXSlider);
    layout->addRow("Dir X", dirXRow);

    connect(dirXSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*100);
        if (dirXSlider->value() != iv)
            dirXSlider->setValue(iv);
        onDirectionChanged();
    });
    connect(dirXSlider, &QSlider::valueChanged, [this](int v){
        double dv = v/100.0;
        if (std::abs(dirXSpin->value() - dv) > 1e-5)
            dirXSpin->setValue(dv);
    });

    auto dirYRow = new QWidget(this);
    auto dirYLayout = new QHBoxLayout(dirYRow);
    dirYSpin = new QDoubleSpinBox(this);
    dirYSpin->setRange(dirMin, dirMax);
    dirYSpin->setSingleStep(dirStep);

    dirYSlider = new QSlider(Qt::Horizontal, this);
    dirYSlider->setRange(int(dirMin*100), int(dirMax*100)); // skala *100

    dirYLayout->addWidget(dirYSpin);
    dirYLayout->addWidget(dirYSlider);
    layout->addRow("Dir Y", dirYRow);

    connect(dirYSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*100);
        if (dirYSlider->value() != iv)
            dirYSlider->setValue(iv);
        onDirectionChanged();
    });
    connect(dirYSlider, &QSlider::valueChanged, [this](int v){
        double dv = v/100.0;
        if (std::abs(dirYSpin->value() - dv) > 1e-5)
            dirYSpin->setValue(dv);
    });


    auto dirZRow = new QWidget(this);
    auto dirZLayout = new QHBoxLayout(dirZRow);
    dirZSpin = new QDoubleSpinBox(this);
    dirZSpin->setRange(dirMin, dirMax);
    dirZSpin->setSingleStep(dirStep);

    dirZSlider = new QSlider(Qt::Horizontal, this);
    dirZSlider->setRange(int(dirMin*100), int(dirMax*100)); // skala *100

    dirZLayout->addWidget(dirZSpin);
    dirZLayout->addWidget(dirZSlider);
    layout->addRow("Dir Z", dirZRow);

    connect(dirZSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*100);
        if (dirZSlider->value() != iv)
            dirZSlider->setValue(iv);
        onDirectionChanged();
    });
    connect(dirZSlider, &QSlider::valueChanged, [this](int v){
        double dv = v/100.0;
        if (std::abs(dirZSpin->value() - dv) > 1e-5)
            dirZSpin->setValue(dv);
    });

    setLayout(layout);
}

void DistantLightPropertiesWidget::setObject(std::shared_ptr<Object3D> object)
{
    light = std::dynamic_pointer_cast<DistantLight>(object);
    if (!light) return;
    lightPropertiesWidget->setObject(light);

    dirXSpin->blockSignals(true);
    dirYSpin->blockSignals(true);
    dirZSpin->blockSignals(true);
    dirXSpin->setValue(light->direction.x);
    dirYSpin->setValue(light->direction.y);
    dirZSpin->setValue(light->direction.z);
    dirXSpin->blockSignals(false);
    dirYSpin->blockSignals(false);
    dirZSpin->blockSignals(false);

    dirXSlider->blockSignals(true);
    dirYSlider->blockSignals(true);
    dirZSlider->blockSignals(true);
    dirXSlider->setValue(light->direction.x);
    dirYSlider->setValue(light->direction.y);
    dirZSlider->setValue(light->direction.z);
    dirXSlider->blockSignals(false);
    dirYSlider->blockSignals(false);
    dirZSlider->blockSignals(false);
}

void DistantLightPropertiesWidget::onDirectionChanged() {
    if (!light) return;
    Vector3 dir(dirXSpin->value(), dirYSpin->value(), dirZSpin->value());
    if (dir.length() > 0.0001)
        light->direction = dir.normalize();
    emit objectChanged();
}
