#include "UI/LightPropertiesWidget.h"
#include <QFormLayout>
#include <cmath>
#include "Rendering/Colors.h"
#include "Math/Vectors.h"
#include "Math/Utility.h"

static inline int snapPcfSamples(int v) {
    if (v < (8+12)/2) return 8;
    if (v < (12+16)/2) return 12;
    return 16;
}

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
    intensitySlider->setRange(int(intensityMin*intensitySliderFactor), int(intensityMax*intensitySliderFactor));
    intensityLayout->addWidget(intensitySpin);
    intensityLayout->addWidget(intensitySlider);
    layout->addRow("Intensity", intensityRow);
    connect(intensitySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*intensitySliderFactor);
        if (intensitySlider->value() != iv)
            intensitySlider->setValue(iv);
        onIntensityChanged(v);
    });
    connect(intensitySlider, &QSlider::valueChanged, [this](int v){
        double dv = double(v)/double(intensitySliderFactor);
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
    biasSlider->setRange(int(biasMin * biasSliderFactor), int(biasMax * biasSliderFactor));
    biasLayout->addWidget(biasSpin);
    biasLayout->addWidget(biasSlider);
    layout->addRow("Shadow bias", biasRow);
    connect(biasSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v * biasSliderFactor);
        if (biasSlider->value() != iv)
            biasSlider->setValue(iv);
        onBiasChanged(v);
    });
    connect(biasSlider, &QSlider::valueChanged, [this](int v){
        double dv = double(v) / double(biasSliderFactor);
        if (std::abs(biasSpin->value() - dv) > 1e-8)
            biasSpin->setValue(dv);
    });

    auto shadowRow = new QWidget(this);
    auto shadowLayout = new QHBoxLayout(shadowRow);
    shadowMapSizeSpin = new QSpinBox(this);
    shadowMapSizeSpin->setRange(shadowMapSizeMin, shadowMapSizeMax);
    shadowMapSizeSpin->setSingleStep(shadowMapSizeStep);
    shadowMapSizeSlider = new QSlider(Qt::Horizontal, this);
    shadowMapSizeSlider->setRange(shadowMapSizeMin, shadowMapSizeMax);
    shadowMapSizeSlider->setSingleStep(shadowMapSizeStep);
    shadowLayout->addWidget(shadowMapSizeSpin);
    shadowLayout->addWidget(shadowMapSizeSlider);
    layout->addRow("Shadow map size", shadowRow);
    connect(shadowMapSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int v){
        if (shadowMapSizeSlider->value() != v)
            shadowMapSizeSlider->setValue(v);
        onShadowMapSizeChanged(v);
    });
    connect(shadowMapSizeSlider, &QSlider::valueChanged, [this](int v){
        if (shadowMapSizeSpin->value() != v)
            shadowMapSizeSpin->setValue(v);
    });

    filteringPropertiesWidget = new LightFilteringPropertiesWidget(this);
    layout->addRow("Filter", filteringPropertiesWidget);
    connect(filteringPropertiesWidget, &ObjectPropertiesWidget::objectChanged, this, &LightPropertiesWidget::objectChanged);

    auto pcfSamplesRow = new QWidget(this);
    auto pcfSamplesLayout = new QHBoxLayout(pcfSamplesRow);
    pcfPoissonSamplesSpin = new QSpinBox(this);
    pcfPoissonSamplesSpin->setRange(pcfSamplesMin, pcfSamplesMax);
    pcfPoissonSamplesSpin->setSingleStep(pcfSamplesStep);
    pcfPoissonSamplesSlider = new QSlider(Qt::Horizontal, this);
    pcfPoissonSamplesSlider->setRange(pcfSamplesMin, pcfSamplesMax);
    pcfPoissonSamplesSlider->setSingleStep(pcfSamplesStep);
    pcfSamplesLayout->addWidget(pcfPoissonSamplesSpin);
    pcfSamplesLayout->addWidget(pcfPoissonSamplesSlider);
    layout->addRow("PCF Poisson samples", pcfSamplesRow);
    connect(pcfPoissonSamplesSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int v){
        int snapped = snapPcfSamples(v);
        if (pcfPoissonSamplesSpin->value() != snapped)
            pcfPoissonSamplesSpin->setValue(snapped);
        if (pcfPoissonSamplesSlider->value() != snapped)
            pcfPoissonSamplesSlider->setValue(snapped);
        onPcfPoissonSamplesChanged(snapped);
    });
    connect(pcfPoissonSamplesSlider, &QSlider::valueChanged, [this](int v){
        int snapped = snapPcfSamples(v);
        if (pcfPoissonSamplesSlider->value() != snapped)
            pcfPoissonSamplesSlider->setValue(snapped);
        if (pcfPoissonSamplesSpin->value() != snapped)
            pcfPoissonSamplesSpin->setValue(snapped);
    });

    auto pcssPenumbraRow = new QWidget(this);
    auto pcssPenumbraLayout = new QHBoxLayout(pcssPenumbraRow);
    pcssPenumbraSpin = new QDoubleSpinBox(this);
    pcssPenumbraSpin->setRange(pcssPenumbraMin, pcssPenumbraMax);
    pcssPenumbraSpin->setSingleStep(pcssPenumbraStep);
    pcssPenumbraSlider = new QSlider(Qt::Horizontal, this);
    pcssPenumbraSlider->setRange(int(pcssPenumbraMin*pcssPenumbraFactor), int(pcssPenumbraMax*pcssPenumbraFactor));
    pcssPenumbraLayout->addWidget(pcssPenumbraSpin);
    pcssPenumbraLayout->addWidget(pcssPenumbraSlider);
    layout->addRow("PCSS penumbra scale", pcssPenumbraRow);
    connect(pcssPenumbraSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*pcssPenumbraFactor);
        if (pcssPenumbraSlider->value() != iv)
            pcssPenumbraSlider->setValue(iv);
        onPcssPenumbraChanged(v);
    });
    connect(pcssPenumbraSlider, &QSlider::valueChanged, [this](int v){
        double dv = double(v)/double(pcssPenumbraFactor);
        if (std::abs(pcssPenumbraSpin->value() - dv) > 1.0/double(pcssPenumbraFactor)/10.0)
            pcssPenumbraSpin->setValue(dv);
    });

    auto pcfRadiusRow = new QWidget(this);
    auto pcfRadiusLayout = new QHBoxLayout(pcfRadiusRow);
    pcfPoissonRadiusSpin = new QDoubleSpinBox(this);
    pcfPoissonRadiusSpin->setRange(pcfRadiusMin, pcfRadiusMax);
    pcfPoissonRadiusSpin->setSingleStep(pcfRadiusStep);
    pcfPoissonRadiusSlider = new QSlider(Qt::Horizontal, this);
    pcfPoissonRadiusSlider->setRange(int(pcfRadiusMin*pcfRadiusFactor), int(pcfRadiusMax*pcfRadiusFactor));
    pcfRadiusLayout->addWidget(pcfPoissonRadiusSpin);
    pcfRadiusLayout->addWidget(pcfPoissonRadiusSlider);
    layout->addRow("PCF Poisson radius (texels)", pcfRadiusRow);
    connect(pcfPoissonRadiusSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double v){
        int iv = int(v*pcfRadiusFactor);
        if (pcfPoissonRadiusSlider->value() != iv)
            pcfPoissonRadiusSlider->setValue(iv);
        onPcfPoissonRadiusChanged(v);
    });
    connect(pcfPoissonRadiusSlider, &QSlider::valueChanged, [this](int v){
        double dv = double(v)/double(pcfRadiusFactor);
        if (std::abs(pcfPoissonRadiusSpin->value() - dv) > 1.0/double(pcfRadiusFactor)/10.0)
            pcfPoissonRadiusSpin->setValue(dv);
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

    filteringPropertiesWidget->setObject(light);

    intensitySpin->blockSignals(true);
    intensitySpin->setValue(light->intensity);
    intensitySpin->blockSignals(false);

    intensitySlider->blockSignals(true);
    intensitySlider->setValue(int(light->intensity*intensitySliderFactor));
    intensitySlider->blockSignals(false);

    biasSpin->blockSignals(true);
    biasSpin->setValue(light->bias);
    biasSpin->blockSignals(false);

    biasSlider->blockSignals(true);
    biasSlider->setValue(int(light->bias*biasSliderFactor));
    biasSlider->blockSignals(false);

    shadowMapSizeSpin->blockSignals(true);
    shadowMapSizeSpin->setValue(light->getShadowMap().width());
    shadowMapSizeSpin->blockSignals(false);

    shadowMapSizeSlider->blockSignals(true);
    shadowMapSizeSlider->setValue(light->getShadowMap().width());
    shadowMapSizeSlider->blockSignals(false);

    pcfPoissonSamplesSpin->blockSignals(true);
    pcfPoissonSamplesSpin->setValue(light->pcfPoissonSamples);
    pcfPoissonSamplesSpin->blockSignals(false);

    pcfPoissonSamplesSlider->blockSignals(true);
    pcfPoissonSamplesSlider->setValue(light->pcfPoissonSamples);
    pcfPoissonSamplesSlider->blockSignals(false);

    pcssPenumbraSpin->blockSignals(true);
    pcssPenumbraSpin->setValue(light->pcssPenumbraScale);
    pcssPenumbraSpin->blockSignals(false);

    pcssPenumbraSlider->blockSignals(true);
    pcssPenumbraSlider->setValue(int(light->pcssPenumbraScale*pcssPenumbraFactor));
    pcssPenumbraSlider->blockSignals(false);

    pcfPoissonRadiusSpin->blockSignals(true);
    pcfPoissonRadiusSpin->setValue(light->pcfPoissonRadiusTexels);
    pcfPoissonRadiusSpin->blockSignals(false);

    pcfPoissonRadiusSlider->blockSignals(true);
    pcfPoissonRadiusSlider->setValue(int(light->pcfPoissonRadiusTexels*pcfRadiusFactor));
    pcfPoissonRadiusSlider->blockSignals(false);

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
    if (light) light->color = Vectors::vector3ToColor(RendUt::sRGBToLinear(Vectors::colorToVector3(color)));
    emit objectChanged();
}

void LightPropertiesWidget::onCastShadowChanged(int state) {
    if (light) light->castsShadow = (state != 0);
    emit objectChanged();
}

void LightPropertiesWidget::onShadowMapSizeChanged(int v) {
    if (light) light->setShadowMapSize(static_cast<size_t>(v));
    emit objectChanged();
}

void LightPropertiesWidget::onPcfPoissonSamplesChanged(int v) {
    if (!light) return;
    light->pcfPoissonSamples = snapPcfSamples(v);
    emit objectChanged();
}

void LightPropertiesWidget::onPcssPenumbraChanged(double v) {
    if (!light) return;
    light->pcssPenumbraScale = v;
    emit objectChanged();
}

void LightPropertiesWidget::onPcfPoissonRadiusChanged(double v) {
    if (!light) return;
    light->pcfPoissonRadiusTexels = v;
    emit objectChanged();
}
