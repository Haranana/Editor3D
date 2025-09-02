#ifndef POINTLIGHTPROPERTIESWIDGET_H
#define POINTLIGHTPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/PointLight.h"
#include "UI/LightPropertiesWidget.h"
#include <QFormLayout>
#include "UI/Vector3PropertiesWidget.h"
#include "Transform3DPropertiesWidget.h"
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

    //void onPosChanged();
    void onRangeChanged(double v);
    void onDynamicBiasChanged();
    void onAttenuationConstantChanged(double v);
    void onAttenuationLinearChanged(double v);
    void onAttenuationQuadraticChanged(double v);

private:

    LightPropertiesWidget* lightPropertiesWidget;
    //Vector3PropertiesWidget* posPropertiesWidget;
    Transform3DPropertiesWidget* transform3DPropertiesWidget;

    //Vector3 posProxy;

    std::shared_ptr<PointLight> light;

    //static constexpr double posRangeMin = -500;
    //static constexpr double posRangeMax = 500;
    //static constexpr double posSpinStep = 1.0;

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
    QCheckBox* dynamicBiasCheckBox;
    QDoubleSpinBox* attenuationConstantSpin;
    QDoubleSpinBox* attenuationLinearSpin;
    QDoubleSpinBox* attenuationQuadraticSpin;

};

#endif // POINTLIGHTPROPERTIESWIDGET_H
