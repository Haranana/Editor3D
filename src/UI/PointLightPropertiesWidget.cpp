#include "UI/PointLightPropertiesWidget.h"

PointLightPropertiesWidget::PointLightPropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    posPropertiesWidget = new Vector3PropertiesWidget(this, posRangeMin, posRangeMax, posSpinStep, 1.0);
    connect(posPropertiesWidget, &Vector3PropertiesWidget::valueChanged, this, &PointLightPropertiesWidget::onPosChanged);
    layout->addRow(posPropertiesWidget);

    lightPropertiesWidget = new LightPropertiesWidget();
    layout->addRow(lightPropertiesWidget);

    auto rangeRow = new QWidget(this);
    auto rangeLayout = new QHBoxLayout(rangeRow);

    rangeSpin = new QDoubleSpinBox(this);
    rangeSpin -> setRange(minRange, maxRange);
    rangeSpin -> setSingleStep(rangeStep);

    rangeSlider = new QSlider(Qt::Horizontal, this);
    rangeSlider->setRange(minRange , maxRange);

    rangeLayout->addWidget(rangeSpin);
    rangeLayout->addWidget(rangeSlider);
    layout->addRow("Range: " , rangeRow);

    connect(rangeSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        if (rangeSlider->value() != v)
            rangeSlider->setValue(v);
        onRangeChanged(v);
    });
    connect(rangeSlider, &QSlider::valueChanged, [this](double v){
        if (rangeSpin->value() != v)
            rangeSpin->setValue(v);
    });


    auto attConstantRow = new QWidget(this);
    auto attConstantLayout = new QHBoxLayout(attConstantRow);

    attenuationConstantSpin = new QDoubleSpinBox(this);
    attenuationConstantSpin->setRange(minAttenuationConstant, maxAttenuationConstant);
    attenuationConstantSpin->setSingleStep(attenuationConstantStep);

    attConstantLayout->addWidget(attenuationConstantSpin);
    layout->addRow("Attenuation constant: " , attConstantRow);

    connect(attenuationConstantSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        onAttenuationConstantChanged(v);
    });



    auto attLinearRow = new QWidget(this);
    auto attLinearLayout = new QHBoxLayout(attLinearRow);

    attenuationLinearSpin = new QDoubleSpinBox(this);
    attenuationLinearSpin->setRange(minAttenuationLinear, maxAttenuationLinear);
    attenuationLinearSpin->setSingleStep(attenuationLinearStep);

    attLinearLayout->addWidget(attenuationLinearSpin);
    layout->addRow("Attenuation Linear: " , attLinearRow);

    connect(attenuationLinearSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        onAttenuationLinearChanged(v);
    });



    auto attQuadraticRow = new QWidget(this);
    auto attQuadraticLayout = new QHBoxLayout(attQuadraticRow);

    attenuationQuadraticSpin = new QDoubleSpinBox(this);
    attenuationQuadraticSpin->setRange(minAttenuationQuadratic, maxAttenuationQuadratic);
    attenuationQuadraticSpin->setSingleStep(attenuationQuadraticStep);

    attQuadraticLayout->addWidget(attenuationQuadraticSpin);
    layout->addRow("Attenuation Quadratic: " , attQuadraticRow);

    connect(attenuationQuadraticSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        onAttenuationQuadraticChanged(v);
    });



    setLayout(layout);
}

void PointLightPropertiesWidget::setObject(std::shared_ptr<Object3D> object)
{
    light = std::dynamic_pointer_cast<PointLight>(object);
    if (!light) return;
    lightPropertiesWidget->setObject(light);

    posProxy = light->transform.getPosition();
    posPropertiesWidget->setVector(posProxy);

    rangeSpin->blockSignals(true);
    rangeSpin->setValue(light->range);
    rangeSpin->blockSignals(false);

    rangeSlider->blockSignals(true);
    rangeSlider->setValue(light->range);
    rangeSlider->blockSignals(false);

    attenuationConstantSpin->blockSignals(true);
    attenuationConstantSpin->setValue(light->attenuationConstant);
    attenuationConstantSpin->blockSignals(false);

    attenuationLinearSpin->blockSignals(true);
    attenuationLinearSpin->setValue(light->attenuationLinear);
    attenuationLinearSpin->blockSignals(false);

    attenuationQuadraticSpin->blockSignals(true);
    attenuationQuadraticSpin->setValue(light->attenuationQuadratic);
    attenuationQuadraticSpin->blockSignals(false);
}

void PointLightPropertiesWidget::onPosChanged(){
    if(!light) return;
    light->transform.setPosition(posProxy);
    emit objectChanged();
}

void PointLightPropertiesWidget::onRangeChanged(double v) {
    if (light) light->range = v;
    emit objectChanged();
}
void PointLightPropertiesWidget::onAttenuationConstantChanged(double v) {
    if (light) light->attenuationConstant = v;
    emit objectChanged();
}
void PointLightPropertiesWidget::onAttenuationLinearChanged(double v) {
    if (light) light->attenuationLinear = v;
    emit objectChanged();
}
void PointLightPropertiesWidget::onAttenuationQuadraticChanged(double v) {
    if (light) light->attenuationQuadratic = v;
    emit objectChanged();
}
