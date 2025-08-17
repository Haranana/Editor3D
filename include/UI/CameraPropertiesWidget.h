#ifndef CAMERAPROPERTIESWIDGET_H
#define CAMERAPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/Object3D.h"
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QHBoxLayout>

class Camera;
class Transform3DPropertiesWidget;

class CameraPropertiesWidget : public ObjectPropertiesWidget{
    Q_OBJECT
public:
    CameraPropertiesWidget(QWidget* parent = nullptr);
    void setObject(std::shared_ptr<Object3D> object) override;

private slots:
    void onFovYChanged(double v);
    void onNearChanged(double v);
    void onFarChanged(double v);

private:
    static constexpr double fovMin = 1.0;
    static constexpr double fovMax = 179.0;
    static constexpr double fovStep = 1.0;

    static constexpr double nearMin = 0.01;
    static constexpr double nearMax = 10.0;
    static constexpr double nearStep = 0.01;

    static constexpr double farMin = 1.0;
    static constexpr double farMax = 5000.0;
    static constexpr double farStep = 1.0;

    std::shared_ptr<Camera> cam;

    Transform3DPropertiesWidget* transformWidget;

    QDoubleSpinBox* fovSpin;
    QSlider* fovSlider;

    QDoubleSpinBox* nearSpin;
    QSlider* nearSlider;

    QDoubleSpinBox* farSpin;
    QSlider* farSlider;
};

#endif // CAMERAPROPERTIESWIDGET_H
