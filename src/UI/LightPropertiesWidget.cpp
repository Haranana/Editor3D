#include "UI/LightPropertiesWidget.h"
#include <QFormLayout>
#include <cmath>

LightPropertiesWidget::LightPropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    auto intensityRow = new QWidget(this);
    auto intensityLayout = new QHBoxLayout(intensityRow);
    intensitySpin = new QDoubleSpinBox(this);
    intensitySpin->setRange(intensityMin, intensityMax);
    intensitySpin->setSingleStep(intensityStep);
    intensitySlider = new QSlider(Qt::Horizontal, this);
    intensitySlider->setRange(int(intensityMin*100), int(intensityMax*100));
    intensityLayout->addWidget(intensitySpin);
    intensityLayout->addWidget(intensitySlider);
    layout->addRow("Intensity", intensityRow);
    connect(intensitySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*100);
        if (intensitySlider->value() != iv)
            intensitySlider->setValue(iv);
        onIntensityChanged(v);
    });
    connect(intensitySlider, &QSlider::valueChanged, [this](int v){
        double dv = v/100.0;
        if (std::abs(intensitySpin->value() - dv) > 0.00001)
            intensitySpin->setValue(dv);
    });

    auto biasRow = new QWidget(this);
    auto biasLayout = new QHBoxLayout(biasRow);
    biasSpin = new QDoubleSpinBox(this);
    biasSpin->setDecimals(6);
    biasSpin->setRange(biasMin, biasMax);
    biasSpin->setSingleStep(biasStep);
    biasSlider = new QSlider(Qt::Horizontal, this);
    biasSlider->setRange(int(biasMin * 1000000), int(biasMax * 1000000));
    biasLayout->addWidget(biasSpin);
    biasLayout->addWidget(biasSlider);
    layout->addRow("Shadow bias", biasRow);
    connect(biasSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v * 1000000);
        if (biasSlider->value() != iv)
            biasSlider->setValue(iv);
        onBiasChanged(v);
    });
    connect(biasSlider, &QSlider::valueChanged, [this](int v){
        double dv = v / 1000000.0;
        if (std::abs(biasSpin->value() - dv) > 1e-8)
            biasSpin->setValue(dv);
    });

    colorPicker = new ColorPicker(this);
    layout->addRow("Color", colorPicker);
    connect(colorPicker, &ColorPicker::colorChanged, this, &LightPropertiesWidget::onColorChanged);

    castShadowCheck = new QCheckBox(this);
    layout->addRow("Casts shadow", castShadowCheck);
    connect(castShadowCheck, &QCheckBox::checkStateChanged, this, &LightPropertiesWidget::onCastShadowChanged);

    setLayout(layout);
}

void LightPropertiesWidget::setObject(std::shared_ptr<Object3D> object)
{
    light = std::dynamic_pointer_cast<Light>(object);
    if (!light) return;

    intensitySpin->blockSignals(true);
    intensitySpin->setValue(light->intensity);
    intensitySpin->blockSignals(false);

    intensitySlider->blockSignals(true);
    intensitySlider->setValue(int(light->intensity*100));
    intensitySlider->blockSignals(false);

    biasSpin->blockSignals(true);
    biasSpin->setValue(light->bias);
    biasSpin->blockSignals(false);

    biasSlider->blockSignals(true);
    biasSlider->setValue(int(light->bias*1000000));
    biasSlider->blockSignals(false);

    colorPicker->blockSignals(true);
    colorPicker->setColor(light->color);
    colorPicker->blockSignals(false);

    castShadowCheck->blockSignals(true);
    castShadowCheck->setChecked(light->castsShadow);
    castShadowCheck->blockSignals(false);
}

void LightPropertiesWidget::onIntensityChanged(double v) {
    if (light) light->intensity = v;
    emit objectChanged();
}

void LightPropertiesWidget::onBiasChanged(double v) {
    if (light) light->bias = v;
    emit objectChanged();
}

void LightPropertiesWidget::onColorChanged(const Color& color) {
    if (light) light->color = color;
    emit objectChanged();
}

void LightPropertiesWidget::onCastShadowChanged(int state) {
    if (light) light->castsShadow = (state != 0);
    emit objectChanged();
}
