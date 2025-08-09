#ifndef SPOTLIGHTPROPERTIESWIDGET_H
#define SPOTLIGHTPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/SpotLight.h"
#include "UI/LightPropertiesWidget.h"
#include "UI/Transform3DPropertiesWidget.h"
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>

class SpotLightPropertiesWidget : public ObjectPropertiesWidget {
    Q_OBJECT
public:
    SpotLightPropertiesWidget(QWidget* parent = nullptr);

    void setObject(std::shared_ptr<Object3D> object) override;

private slots:


    void onDirectionChanged();
    void onOuterAngleChanged();
    void onInnerAngleChanged();
    void onRangeChanged(double v);
    void onAttenuationConstantChanged(double v);
    void onAttenuationLinearChanged(double v);
    void onAttenuationQuadraticChanged(double v);

private:

    LightPropertiesWidget* lightPropertiesWidget;
    Transform3DPropertiesWidget* transform3DPropertiesWidget;
    std::shared_ptr<SpotLight> light;

    static constexpr double dirMin = -1.0;
    static constexpr double dirMax = 1.0;
    static constexpr double dirStep = 0.01;
    QDoubleSpinBox* dirXSpin;
    QDoubleSpinBox* dirYSpin;
    QDoubleSpinBox* dirZSpin;
    QSlider* dirXSlider;
    QSlider* dirYSlider;
    QSlider* dirZSlider;

    static constexpr double outerAngleMin = 0.0;
    static constexpr double outerAngleMax = 360.0;
    static constexpr double outerAngleStep = 1.0;
    static constexpr int outerAngleDecimals = 1;
    QDoubleSpinBox* outerAngleSpin;
    QSlider* outerAngleSlider;

    static constexpr double innerAngleMin = 0.0;
    static constexpr double innerAngleMax = 360.0;
    static constexpr double innerAngleStep = 1.0;
    static constexpr int innerAngleDecimals = 1;
    QDoubleSpinBox* innerAngleSpin;
    QSlider* innerAngleSlider;

    static constexpr double minRange = 1.0;
    static constexpr double maxRange = 1000.0;
    static constexpr double rangeStep = 1.0;
    QDoubleSpinBox* rangeSpin;
    QSlider* rangeSlider;

    static constexpr double minAttenuationConstant = 0.001f;
    static constexpr double maxAttenuationConstant = 1.0f;
    static constexpr double attenuationConstantStep = 0.001f;
    QDoubleSpinBox* attenuationConstantSpin;

    static constexpr double minAttenuationLinear = 0.001f;
    static constexpr double maxAttenuationLinear= 1.0f;
    static constexpr double attenuationLinearStep = 0.001f;
    QDoubleSpinBox* attenuationLinearSpin;

    static constexpr double minAttenuationQuadratic = 0.001f;
    static constexpr double maxAttenuationQuadratic= 1.0f;
    static constexpr double attenuationQuadraticStep = 0.001f;
    QDoubleSpinBox* attenuationQuadraticSpin;

};

#endif // SPOTLIGHTPROPERTIESWIDGET_H
