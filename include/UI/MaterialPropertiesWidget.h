#ifndef MATERIALPROPERTIESWIDGET_H
#define MATERIALPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/RenderableObject3D.h"
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>
#include "Math/Vector3.h"

class Vector3PropertiesWidget;

class MaterialPropertiesWidget : public ObjectPropertiesWidget {
    Q_OBJECT
public:
    MaterialPropertiesWidget(QWidget* parent = nullptr);
    void setObject(std::shared_ptr<Object3D> object) override;

private slots:
    void onAmbientChanged();
    void onDiffuseChanged();
    void onSpecularChanged();
    void onEmissiveChanged();
    void onShininessChanged(double v);
    void onIorChanged(double v);
    void onRoughnessChanged(double v);
    void onMetallicChanged(double v);
    void onOpacityChanged(double v);

private:
    static constexpr double vecMin = 0.0;
    static constexpr double vecMax = 1.0;
    static constexpr double vecStep = 0.01;
    static constexpr double vecSliderFactor = 100.0;
    static constexpr int vecDecimals = 3;

    static constexpr double shininessMin = 1.0;
    static constexpr double shininessMax = 512.0;
    static constexpr double shininessStep = 1.0;

    static constexpr double roughnessMin = 0.05;
    static constexpr double roughnessMax = 1.0;
    static constexpr double roughnessStep = 0.01;

    static constexpr double metallicMin = 0.0;
    static constexpr double metallicMax = 1.0;
    static constexpr double metallicStep = 0.01;

    static constexpr double iorMin = 1.0;
    static constexpr double iorMax = 2.5;
    static constexpr double iorStep = 0.01;

    static constexpr double opacityMin = 0.0;
    static constexpr double opacityMax = 1.0;
    static constexpr double opacityStep = 0.01;

    std::shared_ptr<RenderableObject3D> obj;

    Vector3PropertiesWidget* ambientWidget;
    Vector3PropertiesWidget* diffuseWidget;
    Vector3PropertiesWidget* specularWidget;
    Vector3PropertiesWidget* emissiveWidget;

    Vector3 ambientProxy;
    Vector3 diffuseProxy;
    Vector3 specularProxy;
    Vector3 emissiveProxy;

    QDoubleSpinBox* shininessSpin;
    QSlider* shininessSlider;

    QDoubleSpinBox* iorSpin;
    QSlider* iorSlider;

    QDoubleSpinBox* roughnessSpin;
    QSlider* roughnessSlider;

    QDoubleSpinBox* metallicSpin;
    QSlider* metallicSlider;

    QDoubleSpinBox* opacitySpin;
    QSlider* opacitySlider;
};

#endif // MATERIALPROPERTIESWIDGET_H
