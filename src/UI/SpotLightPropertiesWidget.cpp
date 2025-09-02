#include "UI/SpotLightPropertiesWidget.h"
#include <cmath>
#include "Math/Utility.h"


SpotLightPropertiesWidget::SpotLightPropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    lightPropertiesWidget = new LightPropertiesWidget(this);
    layout->addRow(lightPropertiesWidget);
    connect(lightPropertiesWidget, &LightPropertiesWidget::objectChanged, this, [this]() {
        emit objectChanged();
    });

    transform3DPropertiesWidget = new Transform3DPropertiesWidget(this, true, false, false);
    layout->addRow(transform3DPropertiesWidget);
    connect(transform3DPropertiesWidget, &Transform3DPropertiesWidget::objectChanged, this, [this]() {
        emit objectChanged();
    });

    auto dirXRow = new QWidget(this);
    auto dirXLayout = new QHBoxLayout(dirXRow);
    dirXSpin = new QDoubleSpinBox(this);
    dirXSpin->setRange(dirMin, dirMax);
    dirXSpin->setSingleStep(dirStep);
    dirXSlider = new QSlider(Qt::Horizontal, this);
    dirXSlider->setRange(int(dirMin * 100), int(dirMax * 100));
    dirXLayout->addWidget(dirXSpin);
    dirXLayout->addWidget(dirXSlider);
    layout->addRow("Dir X", dirXRow);

    auto dirYRow = new QWidget(this);
    auto dirYLayout = new QHBoxLayout(dirYRow);
    dirYSpin = new QDoubleSpinBox(this);
    dirYSpin->setRange(dirMin, dirMax);
    dirYSpin->setSingleStep(dirStep);
    dirYSlider = new QSlider(Qt::Horizontal, this);
    dirYSlider->setRange(int(dirMin * 100), int(dirMax * 100));
    dirYLayout->addWidget(dirYSpin);
    dirYLayout->addWidget(dirYSlider);
    layout->addRow("Dir Y", dirYRow);

    auto dirZRow = new QWidget(this);
    auto dirZLayout = new QHBoxLayout(dirZRow);
    dirZSpin = new QDoubleSpinBox(this);
    dirZSpin->setRange(dirMin, dirMax);
    dirZSpin->setSingleStep(dirStep);
    dirZSlider = new QSlider(Qt::Horizontal, this);
    dirZSlider->setRange(int(dirMin * 100), int(dirMax * 100));
    dirZLayout->addWidget(dirZSpin);
    dirZLayout->addWidget(dirZSlider);
    layout->addRow("Dir Z", dirZRow);

    auto bindDir = [this](QDoubleSpinBox* spin, QSlider* slider) {
        connect(spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, [this, slider](double v){
                    int iv = int(std::round(v * 100.0));
                    if (slider->value() != iv) slider->setValue(iv);
                    onDirectionChanged();
                });
        connect(slider, &QSlider::valueChanged,
                this, [this, spin](int v){
                    double dv = v / 100.0;
                    if (!MathUt::equal(spin->value(), dv)) spin->setValue(dv);
                });
    };
    bindDir(dirXSpin, dirXSlider);
    bindDir(dirYSpin, dirYSlider);
    bindDir(dirZSpin, dirZSlider);

    auto dynamicBiasRow = new QWidget(this);
    auto dynamicBiasCheckBoxLayout = new QHBoxLayout(dynamicBiasRow);
    dynamicBiasCheckBox = new QCheckBox(this);
    dynamicBiasCheckBoxLayout->addWidget(dynamicBiasCheckBox);
    layout->addRow("Dynamic bias: ", dynamicBiasRow);
    connect(dynamicBiasCheckBox, QOverload<bool>::of(&QCheckBox::clicked), [this](){
        onDynamicBiasChanged();
    });

    auto outerRow = new QWidget(this);
    auto outerLayout = new QHBoxLayout(outerRow);
    outerAngleSpin = new QDoubleSpinBox(this);
    outerAngleSpin->setRange(outerAngleMin, outerAngleMax);
    outerAngleSpin->setSingleStep(outerAngleStep);
    outerAngleSpin->setDecimals(outerAngleDecimals);
    outerAngleSlider = new QSlider(Qt::Horizontal, this);
    outerAngleSlider->setRange(int(outerAngleMin * 10), int(outerAngleMax * 10));
    outerLayout->addWidget(outerAngleSpin);
    outerLayout->addWidget(outerAngleSlider);
    layout->addRow("Outer half-angle (deg)", outerRow);

    connect(outerAngleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double v){
                int iv = int(std::round(v * 10.0));
                if (outerAngleSlider->value() != iv) outerAngleSlider->setValue(iv);
                onOuterAngleChanged();
            });
    connect(outerAngleSlider, &QSlider::valueChanged,
            this, [this](int v){
                double dv = v / 10.0;
                if (!MathUt::equal(outerAngleSpin->value(), dv)) outerAngleSpin->setValue(dv);

            });

    auto innerRow = new QWidget(this);
    auto innerLayout = new QHBoxLayout(innerRow);
    innerAngleSpin = new QDoubleSpinBox(this);
    innerAngleSpin->setRange(innerAngleMin, innerAngleMax);
    innerAngleSpin->setSingleStep(innerAngleStep);
    innerAngleSpin->setDecimals(innerAngleDecimals);
    innerAngleSlider = new QSlider(Qt::Horizontal, this);
    innerAngleSlider->setRange(int(innerAngleMin * 10), int(innerAngleMax * 10));
    innerLayout->addWidget(innerAngleSpin);
    innerLayout->addWidget(innerAngleSlider);
    layout->addRow("Inner half-angle (deg)", innerRow);

    connect(innerAngleSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double v){
                int iv = int(std::round(v * 10.0));
                if (innerAngleSlider->value() != iv) innerAngleSlider->setValue(iv);
                onInnerAngleChanged();
            });
    connect(innerAngleSlider, &QSlider::valueChanged,
            this, [this](int v){
                double dv = v / 10.0;
                if (!MathUt::equal(innerAngleSpin->value(), dv)) innerAngleSpin->setValue(dv);

            });


    auto rangeRow = new QWidget(this);
    auto rangeLayout = new QHBoxLayout(rangeRow);
    rangeSpin = new QDoubleSpinBox(this);
    rangeSpin->setRange(minRange, maxRange);
    rangeSpin->setSingleStep(rangeStep);
    rangeSlider = new QSlider(Qt::Horizontal, this);
    rangeSlider->setRange(int(minRange), int(maxRange));
    rangeLayout->addWidget(rangeSpin);
    rangeLayout->addWidget(rangeSlider);
    layout->addRow("Range", rangeRow);

    connect(rangeSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double v){
                int iv = int(std::round(v));
                if (rangeSlider->value() != iv) rangeSlider->setValue(iv);
                onRangeChanged(v);
            });
    connect(rangeSlider, &QSlider::valueChanged,
            this, [this](int v){
                double dv = static_cast<double>(v);
                if (!MathUt::equal(rangeSpin->value(), dv)) rangeSpin->setValue(dv);

            });

    attenuationConstantSpin = new QDoubleSpinBox(this);
    attenuationConstantSpin->setRange(minAttenuationConstant, maxAttenuationConstant);
    attenuationConstantSpin->setSingleStep(attenuationConstantStep);
    attenuationConstantSpin->setDecimals(6);
    layout->addRow("Attenuation constant", attenuationConstantSpin);
    connect(attenuationConstantSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SpotLightPropertiesWidget::onAttenuationConstantChanged);

    attenuationLinearSpin = new QDoubleSpinBox(this);
    attenuationLinearSpin->setRange(minAttenuationLinear, maxAttenuationLinear);
    attenuationLinearSpin->setSingleStep(attenuationLinearStep);
    attenuationLinearSpin->setDecimals(6);
    layout->addRow("Attenuation linear", attenuationLinearSpin);
    connect(attenuationLinearSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SpotLightPropertiesWidget::onAttenuationLinearChanged);

    attenuationQuadraticSpin = new QDoubleSpinBox(this);
    attenuationQuadraticSpin->setRange(minAttenuationQuadratic, maxAttenuationQuadratic);
    attenuationQuadraticSpin->setSingleStep(attenuationQuadraticStep);
    attenuationQuadraticSpin->setDecimals(6);
    layout->addRow("Attenuation quadratic", attenuationQuadraticSpin);
    connect(attenuationQuadraticSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &SpotLightPropertiesWidget::onAttenuationQuadraticChanged);

    setLayout(layout);
}

void SpotLightPropertiesWidget::setObject(std::shared_ptr<Object3D> object)
{

    light = std::dynamic_pointer_cast<SpotLight>(object);
    if (!light) return;

    lightPropertiesWidget->setObject(object);
    transform3DPropertiesWidget->setObject(object);

    if(light->biasType == Light::BiasType::CONSTANT){
        dynamicBiasCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }else{
        dynamicBiasCheckBox->setCheckState(Qt::CheckState::Checked);
    }

    dirXSpin->blockSignals(true); dirYSpin->blockSignals(true); dirZSpin->blockSignals(true);
    dirXSlider->blockSignals(true); dirYSlider->blockSignals(true); dirZSlider->blockSignals(true);
    dirXSpin->setValue(light->direction.x);
    dirYSpin->setValue(light->direction.y);
    dirZSpin->setValue(light->direction.z);
    dirXSlider->setValue(int(std::round(light->direction.x * 100.0)));
    dirYSlider->setValue(int(std::round(light->direction.y * 100.0)));
    dirZSlider->setValue(int(std::round(light->direction.z * 100.0)));
    dirXSpin->blockSignals(false); dirYSpin->blockSignals(false); dirZSpin->blockSignals(false);
    dirXSlider->blockSignals(false); dirYSlider->blockSignals(false); dirZSlider->blockSignals(false);

    outerAngleSpin->blockSignals(true);
    outerAngleSlider->blockSignals(true);
    outerAngleSpin->setValue(MathUt::radianToDegree(light->outerAngle));
    outerAngleSlider->setValue(int(std::round(MathUt::radianToDegree(light->outerAngle) * 10.0)));
    outerAngleSpin->blockSignals(false);
    outerAngleSlider->blockSignals(false);

    innerAngleSpin->blockSignals(true);
    innerAngleSlider->blockSignals(true);
    innerAngleSpin->setValue(MathUt::radianToDegree(light->innerAngle));
    innerAngleSlider->setValue(int(std::round(MathUt::radianToDegree(light->innerAngle) * 10.0)));
    innerAngleSpin->blockSignals(false);
    innerAngleSlider->blockSignals(false);

    rangeSpin->blockSignals(true);
    rangeSlider->blockSignals(true);
    rangeSpin->setValue(light->range);
    rangeSlider->setValue(light->range);
    rangeSpin->blockSignals(false);
    rangeSlider->blockSignals(false);

    attenuationConstantSpin->blockSignals(true);
    attenuationLinearSpin->blockSignals(true);
    attenuationQuadraticSpin->blockSignals(true);

    attenuationConstantSpin->setValue(light->attenuationConstant);
    attenuationLinearSpin->setValue(light->attenuationLinear);
    attenuationQuadraticSpin->setValue(light->attenuationQuadratic);

    attenuationConstantSpin->blockSignals(false);
    attenuationLinearSpin->blockSignals(false);
    attenuationQuadraticSpin->blockSignals(false);
}

void SpotLightPropertiesWidget::onDirectionChanged() {
    if (!light) return;
    Vector3 d(dirXSpin->value(), dirYSpin->value(), dirZSpin->value());
    if (d.length() > 1e-6) d = d.normalize();
    light->direction = d;
    emit objectChanged();
}

void SpotLightPropertiesWidget::onDynamicBiasChanged(){
    if(!light) return;
    if(dynamicBiasCheckBox->checkState() == Qt::CheckState::Checked){
        light->biasType = Light::BiasType::NORMAL_ANGLE;
    }else{
        light->biasType = Light::BiasType::CONSTANT;
    }
    emit objectChanged();
}

void SpotLightPropertiesWidget::onOuterAngleChanged() {
    if (!light) return;
    const double halfAngleRad = MathUt::degreeToRadian(outerAngleSpin->value());

    light->outerAngle = std::clamp(halfAngleRad, MathUt::degreeToRadian(0.1), MathUt::degreeToRadian(89.0));
    if (light->innerAngle >= light->outerAngle){
        light->innerAngle = std::max(MathUt::degreeToRadian(0.1), light->outerAngle - MathUt::degreeToRadian(0.1));

    }
    light->updateCos();
    emit objectChanged();
}

void SpotLightPropertiesWidget::onInnerAngleChanged() {
    if (!light) return;
    const double halfAngleRad = MathUt::degreeToRadian(innerAngleSpin->value());
    light->innerAngle = std::clamp(halfAngleRad, 0.0, light->outerAngle - MathUt::degreeToRadian(0.1));
    light->updateCos();
    emit objectChanged();
}

void SpotLightPropertiesWidget::onRangeChanged(double v) {
    if (!light) return;
    light->range = v;
    emit objectChanged();
}

void SpotLightPropertiesWidget::onAttenuationConstantChanged(double v) {
    if (!light) return;
    light->attenuationConstant = v;
    emit objectChanged();
}

void SpotLightPropertiesWidget::onAttenuationLinearChanged(double v) {
    if (!light) return;
    light->attenuationLinear = v;
    emit objectChanged();
}

void SpotLightPropertiesWidget::onAttenuationQuadraticChanged(double v) {
    if (!light) return;
    light->attenuationQuadratic = v;
    emit objectChanged();
}
