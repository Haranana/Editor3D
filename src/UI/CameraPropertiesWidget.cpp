#include "UI/CameraPropertiesWidget.h"
#include "Scene/Camera.h"
#include "UI/Transform3DPropertiesWidget.h"
#include <QFormLayout>
#include <cmath>

CameraPropertiesWidget::CameraPropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    transformWidget = new Transform3DPropertiesWidget(this);
    layout->addRow(transformWidget);

    auto fovRow = new QWidget(this);
    auto fovLayout = new QHBoxLayout(fovRow);
    fovSpin = new QDoubleSpinBox(this);
    fovSpin->setRange(fovMin, fovMax);
    fovSpin->setSingleStep(fovStep);
    fovSlider = new QSlider(Qt::Horizontal, this);
    fovSlider->setRange(int(fovMin), int(fovMax));
    fovLayout->addWidget(fovSpin);
    fovLayout->addWidget(fovSlider);
    layout->addRow("Field of view (Y)", fovRow);
    connect(fovSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v);
        if (fovSlider->value() != iv)
            fovSlider->setValue(iv);
        onFovYChanged(v);
    });
    connect(fovSlider, &QSlider::valueChanged, [this](int v){
        double dv = v;
        if (std::abs(fovSpin->value() - dv) > 1e-9)
            fovSpin->setValue(dv);
    });

    auto nearRow = new QWidget(this);
    auto nearLayout = new QHBoxLayout(nearRow);
    nearSpin = new QDoubleSpinBox(this);
    nearSpin->setDecimals(2);
    nearSpin->setRange(nearMin, nearMax);
    nearSpin->setSingleStep(nearStep);
    nearSlider = new QSlider(Qt::Horizontal, this);
    nearSlider->setRange(int(nearMin*100), int(nearMax*100));
    nearLayout->addWidget(nearSpin);
    nearLayout->addWidget(nearSlider);
    layout->addRow("Near plane", nearRow);
    connect(nearSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*100);
        if (nearSlider->value() != iv)
            nearSlider->setValue(iv);
        onNearChanged(v);
    });
    connect(nearSlider, &QSlider::valueChanged, [this](int v){
        double dv = v/100.0;
        if (std::abs(nearSpin->value() - dv) > 1e-6)
            nearSpin->setValue(dv);
    });

    auto farRow = new QWidget(this);
    auto farLayout = new QHBoxLayout(farRow);
    farSpin = new QDoubleSpinBox(this);
    farSpin->setDecimals(0);
    farSpin->setRange(farMin, farMax);
    farSpin->setSingleStep(farStep);
    farSlider = new QSlider(Qt::Horizontal, this);
    farSlider->setRange(int(farMin), int(farMax));
    farLayout->addWidget(farSpin);
    farLayout->addWidget(farSlider);
    layout->addRow("Far plane", farRow);
    connect(farSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v);
        if (farSlider->value() != iv)
            farSlider->setValue(iv);
        onFarChanged(v);
    });
    connect(farSlider, &QSlider::valueChanged, [this](int v){
        double dv = v;
        if (std::abs(farSpin->value() - dv) > 1e-6)
            farSpin->setValue(dv);
    });

    setLayout(layout);
}

void CameraPropertiesWidget::setObject(std::shared_ptr<Object3D> object)
{
    cam = std::dynamic_pointer_cast<Camera>(object);
    if (!cam) return;

    transformWidget->setObject(cam);

    fovSpin->blockSignals(true);
    fovSpin->setValue(cam->fovY);
    fovSpin->blockSignals(false);
    fovSlider->blockSignals(true);
    fovSlider->setValue(int(cam->fovY));
    fovSlider->blockSignals(false);

    nearSpin->blockSignals(true);
    nearSpin->setValue(cam->nearPlane);
    nearSpin->blockSignals(false);
    nearSlider->blockSignals(true);
    nearSlider->setValue(int(cam->nearPlane*100));
    nearSlider->blockSignals(false);

    farSpin->blockSignals(true);
    farSpin->setValue(cam->farPlane);
    farSpin->blockSignals(false);
    farSlider->blockSignals(true);
    farSlider->setValue(int(cam->farPlane));
    farSlider->blockSignals(false);

    connect(transformWidget, &ObjectPropertiesWidget::objectChanged, this, &CameraPropertiesWidget::objectChanged);
}

void CameraPropertiesWidget::onFovYChanged(double v)
{
    if (!cam) return;
    cam->fovY = v;
    emit objectChanged();
}

void CameraPropertiesWidget::onNearChanged(double v)
{
    if (!cam) return;
    cam->nearPlane = v;
    emit objectChanged();
}

void CameraPropertiesWidget::onFarChanged(double v)
{
    if (!cam) return;
    cam->farPlane = v;
    emit objectChanged();
}
