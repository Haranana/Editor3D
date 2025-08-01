#ifndef DISTANTLIGHTPROPERTIESWIDGET_H
#define DISTANTLIGHTPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/DistantLight.h"
#include "ColorPicker.h"
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>

class DistantLightPropertiesWidget : public ObjectPropertiesWidget {
    Q_OBJECT
public:
    DistantLightPropertiesWidget(QWidget* parent = nullptr);

    void setObject(std::shared_ptr<Object3D> object) override;

private slots:
    void onIntensityChanged(double v);
    void onColorChanged(const Color& color);
    void onBiasChanged(double v);
    void onDirectionChanged();
    void onCastShadowChanged(int state);

private:

    static constexpr double intensityMin = 0.0;
    static constexpr double intensityMax = 10.0;
    static constexpr double intensityStep = 0.01;

    static constexpr double biasMin = 0.00001;
    static constexpr double biasMax = 0.01;
    static constexpr double biasStep = 0.00001;

    static constexpr double dirMin = -1.0;
    static constexpr double dirMax = 1.0;
    static constexpr double dirStep = 0.01;

    std::shared_ptr<DistantLight> light;

    QDoubleSpinBox* intensitySpin;
    QDoubleSpinBox* biasSpin;
    ColorPicker* colorPicker;
    QCheckBox* castShadowCheck;

    QDoubleSpinBox* dirXSpin;
    QDoubleSpinBox* dirYSpin;
    QDoubleSpinBox* dirZSpin;

    QSlider* intensitySlider;
    QSlider* biasSlider;

    QSlider* dirXSlider;
    QSlider* dirYSlider;
    QSlider* dirZSlider;
};

#endif // DISTANTLIGHTPROPERTIESWIDGET_H
