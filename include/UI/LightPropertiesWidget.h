#ifndef LIGHTPROPERTIESWIDGET_H
#define LIGHTPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/Light.h"
#include "ColorPicker.h"
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>

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

private:
    static constexpr double intensityMin = 0.0;
    static constexpr double intensityMax = 10.0;
    static constexpr double intensityStep = 0.01;

    static constexpr double biasMin = 0.0001;
    static constexpr double biasMax = 1.0;
    static constexpr double biasStep = 0.0001;

    static constexpr int intensitySliderFactor = int(1.0/intensityStep);
    static constexpr int biasSliderFactor = int(1.0/biasStep);

    std::shared_ptr<Light> light;

    QDoubleSpinBox* intensitySpin;
    QDoubleSpinBox* biasSpin;
    ColorPicker* colorPicker;
    QCheckBox* castShadowCheck;

    QSlider* intensitySlider;
    QSlider* biasSlider;
};

#endif // LIGHTPROPERTIESWIDGET_H
