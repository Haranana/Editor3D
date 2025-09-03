#ifndef SCENEPROPERTIESWIDGET_H
#define SCENEPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/Scene.h"
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QHBoxLayout>

class Vector3PropertiesWidget;

class ScenePropertiesWidget : public ObjectPropertiesWidget{
    Q_OBJECT
public:
    ScenePropertiesWidget(QWidget* parent = nullptr);
    void setScene(std::shared_ptr<Scene> scene);
    void setObject(std::shared_ptr<Object3D> object) override; //existst only for legacy reasons, should not be used


private slots:
    void onAmbientColorChanged();
    void onAmbientIntensityChanged(double v);

private:
    static constexpr double colorMin = 0.0;
    static constexpr double colorMax = 1.0;
    static constexpr double colorStep = 0.01;
    static constexpr double colorSliderFactor = 100.0;
    static constexpr int colorDecimals = 3;

    static constexpr double intensityMin = 0.0;
    static constexpr double intensityMax = 2.0;
    static constexpr double intensityStep = 0.01;
    static constexpr int intensitySliderFactor = int(1.0 / intensityStep);

    std::shared_ptr<Scene> scene;

    Vector3PropertiesWidget* ambientColorWidget;
    Vector3 ambientColorProxy;

    QDoubleSpinBox* ambientIntensitySpin;
    QSlider* ambientIntensitySlider;
};

#endif // SCENEPROPERTIESWIDGET_H

