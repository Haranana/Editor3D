#include "UI/MaterialPropertiesWidget.h"
#include "UI/Vector3PropertiesWidget.h"
#include <QFormLayout>
#include <cmath>

MaterialPropertiesWidget::MaterialPropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    ambientWidget = new Vector3PropertiesWidget(this, vecMin, vecMax, vecStep, vecSliderFactor, vecDecimals);
    diffuseWidget = new Vector3PropertiesWidget(this, vecMin, vecMax, vecStep, vecSliderFactor, vecDecimals);
    specularWidget = new Vector3PropertiesWidget(this, vecMin, vecMax, vecStep, vecSliderFactor, vecDecimals);
    emissiveWidget = new Vector3PropertiesWidget(this, vecMin, vecMax, vecStep, vecSliderFactor, vecDecimals);

    connect(ambientWidget, &Vector3PropertiesWidget::valueChanged, this, &MaterialPropertiesWidget::onAmbientChanged);
    connect(diffuseWidget, &Vector3PropertiesWidget::valueChanged, this, &MaterialPropertiesWidget::onDiffuseChanged);
    connect(specularWidget, &Vector3PropertiesWidget::valueChanged, this, &MaterialPropertiesWidget::onSpecularChanged);
    connect(emissiveWidget, &Vector3PropertiesWidget::valueChanged, this, &MaterialPropertiesWidget::onEmissiveChanged);

    layout->addRow("Ambient", ambientWidget);
    layout->addRow("Diffuse", diffuseWidget);
    layout->addRow("Specular", specularWidget);
    layout->addRow("Emissive", emissiveWidget);

    auto shininessRow = new QWidget(this);
    auto shininessLayout = new QHBoxLayout(shininessRow);
    shininessSpin = new QDoubleSpinBox(this);
    shininessSpin->setRange(shininessMin, shininessMax);
    shininessSpin->setSingleStep(shininessStep);
    shininessSlider = new QSlider(Qt::Horizontal, this);
    shininessSlider->setRange(int(shininessMin), int(shininessMax));
    shininessLayout->addWidget(shininessSpin);
    shininessLayout->addWidget(shininessSlider);
    layout->addRow("Shininess", shininessRow);
    connect(shininessSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v);
        if (shininessSlider->value() != iv)
            shininessSlider->setValue(iv);
        onShininessChanged(v);
    });
    connect(shininessSlider, &QSlider::valueChanged, [this](int v){
        double dv = v;
        if (std::abs(shininessSpin->value() - dv) > 1e-9)
            shininessSpin->setValue(dv);
    });

    auto iorRow = new QWidget(this);
    auto iorLayout = new QHBoxLayout(iorRow);
    iorSpin = new QDoubleSpinBox(this);
    iorSpin->setDecimals(2);
    iorSpin->setRange(iorMin, iorMax);
    iorSpin->setSingleStep(iorStep);
    iorSlider = new QSlider(Qt::Horizontal, this);
    iorSlider->setRange(int(iorMin*100), int(iorMax*100));
    iorLayout->addWidget(iorSpin);
    iorLayout->addWidget(iorSlider);
    layout->addRow("Index of refraction", iorRow);
    connect(iorSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*100);
        if (iorSlider->value() != iv)
            iorSlider->setValue(iv);
        onIorChanged(v);
    });
    connect(iorSlider, &QSlider::valueChanged, [this](int v){
        double dv = v/100.0;
        if (std::abs(iorSpin->value() - dv) > 1e-6)
            iorSpin->setValue(dv);
    });

    auto opacityRow = new QWidget(this);
    auto opacityLayout = new QHBoxLayout(opacityRow);
    opacitySpin = new QDoubleSpinBox(this);
    opacitySpin->setDecimals(2);
    opacitySpin->setRange(opacityMin, opacityMax);
    opacitySpin->setSingleStep(opacityStep);
    opacitySlider = new QSlider(Qt::Horizontal, this);
    opacitySlider->setRange(int(opacityMin*100), int(opacityMax*100));
    opacityLayout->addWidget(opacitySpin);
    opacityLayout->addWidget(opacitySlider);
    layout->addRow("Opacity", opacityRow);
    connect(opacitySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*100);
        if (opacitySlider->value() != iv)
            opacitySlider->setValue(iv);
        onOpacityChanged(v);
    });
    connect(opacitySlider, &QSlider::valueChanged, [this](int v){
        double dv = v/100.0;
        if (std::abs(opacitySpin->value() - dv) > 1e-6)
            opacitySpin->setValue(dv);
    });

    setLayout(layout);
}

void MaterialPropertiesWidget::setObject(std::shared_ptr<Object3D> object)
{
    obj = std::dynamic_pointer_cast<RenderableObject3D>(object);
    if (!obj) return;

    ambientProxy = obj->material.Ka;
    diffuseProxy = obj->material.Kd;
    specularProxy = obj->material.Ks;
    emissiveProxy = obj->material.Ke;

    ambientWidget->setVector(ambientProxy);
    diffuseWidget->setVector(diffuseProxy);
    specularWidget->setVector(specularProxy);
    emissiveWidget->setVector(emissiveProxy);

    shininessSpin->blockSignals(true);
    shininessSpin->setValue(obj->material.Ns);
    shininessSpin->blockSignals(false);
    shininessSlider->blockSignals(true);
    shininessSlider->setValue(int(obj->material.Ns));
    shininessSlider->blockSignals(false);

    iorSpin->blockSignals(true);
    iorSpin->setValue(obj->material.Ni);
    iorSpin->blockSignals(false);
    iorSlider->blockSignals(true);
    iorSlider->setValue(int(obj->material.Ni*100));
    iorSlider->blockSignals(false);

    opacitySpin->blockSignals(true);
    opacitySpin->setValue(obj->material.d);
    opacitySpin->blockSignals(false);
    opacitySlider->blockSignals(true);
    opacitySlider->setValue(int(obj->material.d*100));
    opacitySlider->blockSignals(false);
}

void MaterialPropertiesWidget::onAmbientChanged()
{
    if (!obj) return;
    obj->material.Ka = ambientProxy;
    emit objectChanged();
}

void MaterialPropertiesWidget::onDiffuseChanged()
{
    if (!obj) return;
    obj->material.Kd = diffuseProxy;
    emit objectChanged();
}

void MaterialPropertiesWidget::onSpecularChanged()
{
    if (!obj) return;
    obj->material.Ks = specularProxy;
    emit objectChanged();
}

void MaterialPropertiesWidget::onEmissiveChanged()
{
    if (!obj) return;
    obj->material.Ke = emissiveProxy;
    emit objectChanged();
}

void MaterialPropertiesWidget::onShininessChanged(double v)
{
    if (!obj) return;
    obj->material.Ns = v;
    emit objectChanged();
}

void MaterialPropertiesWidget::onIorChanged(double v)
{
    if (!obj) return;
    obj->material.Ni = v;
    emit objectChanged();
}

void MaterialPropertiesWidget::onOpacityChanged(double v)
{
    if (!obj) return;
    obj->material.d = v;
    emit objectChanged();
}
