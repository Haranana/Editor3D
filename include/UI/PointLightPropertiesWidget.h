#ifndef POINTLIGHTPROPERTIESWIDGET_H
#define POINTLIGHTPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/PointLight.h"
#include "UI/LightPropertiesWidget.h"
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>

class PointLightPropertiesWidget : public ObjectPropertiesWidget {
    Q_OBJECT
public:
    PointLightPropertiesWidget(QWidget* parent = nullptr);

    void setObject(std::shared_ptr<Object3D> object) override;

private slots:

    void onRangeChanged(double v);
    void onAttenuationConstantChanged(double v);
    void onAttenuationLinearChanged(double v);
    void onAttenuationQuadraticChanged(double v);

private:

    LightPropertiesWidget* lightPropertiesWidget;


    std::shared_ptr<PointLight> light;

    static constexpr double minRange = 1.0;
    static constexpr double maxRange = 1000.0;
    static constexpr double rangeStep = 1.0;

    static constexpr double minAttenuationConstant = 0.001f;
    static constexpr double maxAttenuationConstant = 1.0f;
    static constexpr double attenuationConstantStep = 0.001f;

    static constexpr double minAttenuationLinear = 0.001f;
    static constexpr double maxAttenuationLinear= 1.0f;
    static constexpr double attenuationLinearStep = 0.001f;

    static constexpr double minAttenuationQuadratic = 0.001f;
    static constexpr double maxAttenuationQuadratic= 1.0f;
    static constexpr double attenuationQuadraticStep = 0.001f;

    QDoubleSpinBox* rangeSpin;
    QSlider* rangeSlider;
    QDoubleSpinBox* attenuationConstantSpin;
    QDoubleSpinBox* attenuationLinearSpin;
    QDoubleSpinBox* attenuationQuadraticSpin;

};

#endif // POINTLIGHTPROPERTIESWIDGET_H
