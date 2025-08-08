#ifndef DISTANTLIGHTPROPERTIESWIDGET_H
#define DISTANTLIGHTPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/DistantLight.h"
#include "UI/LightPropertiesWidget.h"
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

    void onDirectionChanged();

private:

    static constexpr double dirMin = -1.0;
    static constexpr double dirMax = 1.0;
    static constexpr double dirStep = 0.01;

    LightPropertiesWidget* lightPropertiesWidget;

    std::shared_ptr<DistantLight> light;

    QDoubleSpinBox* dirXSpin;
    QDoubleSpinBox* dirYSpin;
    QDoubleSpinBox* dirZSpin;

    QSlider* dirXSlider;
    QSlider* dirYSlider;
    QSlider* dirZSlider;
};

#endif // DISTANTLIGHTPROPERTIESWIDGET_H
