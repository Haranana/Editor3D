#include "UI/DistantLightPropertiesWidget.h"

DistantLightPropertiesWidget::DistantLightPropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    auto intensityRow = new QWidget(this);
    auto intensityLayout = new QHBoxLayout(intensityRow);
    intensitySpin = new QDoubleSpinBox(this);
    intensitySpin->setRange(intensityMin, intensityMax);
    intensitySpin->setSingleStep(intensityStep);

    intensitySlider = new QSlider(Qt::Horizontal, this);
    intensitySlider->setRange(int(intensityMin*100), int(intensityMax*100)); // Skala *100

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
    biasSlider->setRange(int(biasMin * 1000000), int(biasMax * 1000000)); // skala do 1e-6

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
    connect(colorPicker, &ColorPicker::colorChanged,
            this, &DistantLightPropertiesWidget::onColorChanged);

    castShadowCheck = new QCheckBox(this);
    layout->addRow("Casts shadow", castShadowCheck);
    connect(castShadowCheck, &QCheckBox::checkStateChanged,
            this, &DistantLightPropertiesWidget::onCastShadowChanged);

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

    intensitySpin->blockSignals(true);
    intensitySpin->setValue(light->intensity);
    intensitySpin->blockSignals(false);

    biasSpin->blockSignals(true);
    biasSpin->setValue(light->bias);
    biasSpin->blockSignals(false);

    colorPicker->blockSignals(true);
    colorPicker->setColor(light->color);
    colorPicker->blockSignals(false);

    castShadowCheck->blockSignals(true);
    castShadowCheck->setChecked(light->castsShadow);
    castShadowCheck->blockSignals(false);

    dirXSpin->blockSignals(true);
    dirYSpin->blockSignals(true);
    dirZSpin->blockSignals(true);
    dirXSpin->setValue(light->direction.x);
    dirYSpin->setValue(light->direction.y);
    dirZSpin->setValue(light->direction.z);
    dirXSpin->blockSignals(false);
    dirYSpin->blockSignals(false);
    dirZSpin->blockSignals(false);

    intensitySpin->blockSignals(true);
    intensitySpin->setValue(light->intensity); intensitySpin->blockSignals(false);
    intensitySlider->blockSignals(true);
    intensitySlider->setValue(int(light->intensity*100)); intensitySlider->blockSignals(false);

    biasSpin->blockSignals(true);
    biasSpin->setValue(light->bias);
    biasSpin->blockSignals(false);

    biasSlider->blockSignals(true);
    biasSlider->setValue(int(light->bias*1000000));
    biasSlider->blockSignals(false);

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

void DistantLightPropertiesWidget::onIntensityChanged(double v) {
    if (light) light->intensity = v;
    emit objectChanged();
}
void DistantLightPropertiesWidget::onBiasChanged(double v) {
    if (light) light->bias = v;
    emit objectChanged();
}
void DistantLightPropertiesWidget::onColorChanged(const Color& color) {
    if (light) light->color = color;
    emit objectChanged();
}
void DistantLightPropertiesWidget::onCastShadowChanged(int state) {
    if (light) light->castsShadow = (state != 0);
    emit objectChanged();
}
void DistantLightPropertiesWidget::onDirectionChanged() {
    if (!light) return;
    Vector3 dir(dirXSpin->value(), dirYSpin->value(), dirZSpin->value());
    if (dir.length() > 0.0001)
        light->direction = dir.normalize();
    emit objectChanged();
}
