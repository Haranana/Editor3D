#ifndef LIGHTPROPERTIESWIDGET_H
#define LIGHTPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "UI/LightFilteringPropertiesWidget.h"
#include "Scene/Light.h"
#include "ColorPicker.h"
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>

class LightPropertiesWidget : public ObjectPropertiesWidget {
    Q_OBJECT
public:
    LightPropertiesWidget(QWidget* parent = nullptr);
    void setObject(std::shared_ptr<Object3D> object) override;

private slots:
    void onIntensityChanged(double v);
    void onColorChanged(const Color& color);
    void onBiasChanged(double v);
    void onCastShadowChanged(int state);
    void onShadowMapSizeChanged(int v);
    void onPcfPoissonSamplesChanged(int v);
    void onPcssPenumbraChanged(double v);
    void onPcfPoissonRadiusChanged(double v);

private:
    static constexpr double intensityMin = 0.0;
    static constexpr double intensityMax = 10.0;
    static constexpr double intensityStep = 0.01;

    static constexpr double biasMin = 0.0001;
    static constexpr double biasMax = 1.0;
    static constexpr double biasStep = 0.0001;

    static constexpr int intensitySliderFactor = int(1.0/intensityStep);
    static constexpr int biasSliderFactor = int(1.0/biasStep);

    static constexpr int shadowMapSizeMin = 128;
    static constexpr int shadowMapSizeMax = 4096;
    static constexpr int shadowMapSizeStep = 32;

    static constexpr int pcfSamplesMin = 8;
    static constexpr int pcfSamplesMax = 16;
    static constexpr int pcfSamplesStep = 4;

    static constexpr double pcssPenumbraMin = 1.0;
    static constexpr double pcssPenumbraMax = 3.0;
    static constexpr double pcssPenumbraStep = 0.01;
    static constexpr int pcssPenumbraFactor = int(1.0/pcssPenumbraStep);

    static constexpr double pcfRadiusMin = 2.0;
    static constexpr double pcfRadiusMax = 5.0;
    static constexpr double pcfRadiusStep = 0.1;
    static constexpr int pcfRadiusFactor = int(1.0/pcfRadiusStep);

    LightFilteringPropertiesWidget* filteringPropertiesWidget;

    std::shared_ptr<Light> light;

    QDoubleSpinBox* intensitySpin;
    QDoubleSpinBox* biasSpin;
    ColorPicker* colorPicker;
    QCheckBox* castShadowCheck;

    QSlider* intensitySlider;
    QSlider* biasSlider;

    QSpinBox* shadowMapSizeSpin;
    QSlider* shadowMapSizeSlider;

    QSpinBox* pcfPoissonSamplesSpin;
    QSlider* pcfPoissonSamplesSlider;

    QDoubleSpinBox* pcssPenumbraSpin;
    QSlider* pcssPenumbraSlider;

    QDoubleSpinBox* pcfPoissonRadiusSpin;
    QSlider* pcfPoissonRadiusSlider;
};

#endif // LIGHTPROPERTIESWIDGET_H
