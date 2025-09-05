#include "UI/ScenePropertiesWidget.h"
#include "UI/Vector3PropertiesWidget.h"
#include <QFormLayout>
#include "Math/Utility.h"
#include <cmath>

ScenePropertiesWidget::ScenePropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    ambientColorWidget = new Vector3PropertiesWidget(this, colorMin, colorMax, colorStep, colorSliderFactor, colorDecimals, true);
    connect(ambientColorWidget, &Vector3PropertiesWidget::valueChanged, this, &ScenePropertiesWidget::onAmbientColorChanged);
    layout->addRow("Ambient color", ambientColorWidget);

    auto intensityRow = new QWidget(this);
    auto intensityLayout = new QHBoxLayout(intensityRow);
    ambientIntensitySpin = new QDoubleSpinBox(this);
    ambientIntensitySpin->setRange(intensityMin, intensityMax);
    ambientIntensitySpin->setSingleStep(intensityStep);
    ambientIntensitySlider = new QSlider(Qt::Horizontal, this);
    ambientIntensitySlider->setRange(int(intensityMin*intensitySliderFactor), int(intensityMax*intensitySliderFactor));
    intensityLayout->addWidget(ambientIntensitySpin);
    intensityLayout->addWidget(ambientIntensitySlider);
    layout->addRow("Ambient intensity", intensityRow);

    connect(ambientIntensitySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(std::round(v*intensitySliderFactor));
        if (ambientIntensitySlider->value() != iv)
            ambientIntensitySlider->setValue(iv);
        onAmbientIntensityChanged(v);
    });
    connect(ambientIntensitySlider, &QSlider::valueChanged, [this](int v){
        double dv = double(v)/double(intensitySliderFactor);
        if (std::abs(ambientIntensitySpin->value() - dv) > 1.0/double(intensitySliderFactor)/10.0)
            ambientIntensitySpin->setValue(dv);
    });

    setLayout(layout);
}

void ScenePropertiesWidget::setScene(std::shared_ptr<Scene> scene)
{
    this->scene = scene;
    if (!this->scene) return;

    ambientColorProxy = this->scene->ambientColor;
    ambientColorWidget->setVector(ambientColorProxy);

    ambientIntensitySpin->blockSignals(true);
    ambientIntensitySpin->setValue(this->scene->ambientIntensity);
    ambientIntensitySpin->blockSignals(false);

    ambientIntensitySlider->blockSignals(true);
    ambientIntensitySlider->setValue(int(std::round(this->scene->ambientIntensity*intensitySliderFactor)));
    ambientIntensitySlider->blockSignals(false);
}


void ScenePropertiesWidget::setObject(std::shared_ptr<Object3D> object){

}

void ScenePropertiesWidget::onAmbientColorChanged()
{
    if (!scene) return;
    scene->ambientColor = RendUt::sRGBToLinear(ambientColorProxy);
    emit objectChanged();
}

void ScenePropertiesWidget::onAmbientIntensityChanged(double v)
{
    if (!scene) return;
    scene->ambientIntensity = v;
    emit objectChanged();
}
