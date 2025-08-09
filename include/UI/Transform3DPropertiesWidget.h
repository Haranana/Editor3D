#ifndef TRANSFORM3DPROPERTIESWIDGET_H
#define TRANSFORM3DPROPERTIESWIDGET_H
#include "Scene/RenderableObject3D.h"
#include "ObjectPropertiesWidget.h"
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QComboBox>
#include "Math/Utility.h"
#include <QScrollArea>
#include <QCheckBox>

class Transform3DPropertiesWidget : public ObjectPropertiesWidget{
    Q_OBJECT
public:

    Transform3DPropertiesWidget(QWidget* parent = nullptr);

    void setObject(std::shared_ptr<Object3D> object) override;

private slots:
    void onPosChanged();
    void onScaleChanged();
    void onRotChanged();

private:

    static constexpr double posRangeMin = -500;
    static constexpr double posRangeMax = 500;
    static constexpr double posSpinStep = 1.0;

    static constexpr double scaleRangeMin = 0.1;
    static constexpr double scaleRangeMax = 5.0;
    static constexpr double scaleSpinStep = 0.01;
    static constexpr int scaleSliderFactor = 100;

    static constexpr double rotRangeMin = 0.0;
    static constexpr double rotRangeMax = 360.0;
    static constexpr double rotSpinStep = 1.0;
    static constexpr int rotSpinDecimals = 1;

    std::shared_ptr<Object3D> obj;



    QDoubleSpinBox* posX;
    QDoubleSpinBox* posY;
    QDoubleSpinBox* posZ;
    QSlider* posXSlider;
    QSlider* posYSlider;
    QSlider* posZSlider;

    QDoubleSpinBox* scaleX;
    QDoubleSpinBox* scaleY;
    QDoubleSpinBox* scaleZ;
    QSlider* scaleXSlider;
    QSlider* scaleYSlider;
    QSlider* scaleZSlider;

    QDoubleSpinBox* rotX;
    QDoubleSpinBox* rotY;
    QDoubleSpinBox* rotZ;
    QSlider* rotXSlider;
    QSlider* rotYSlider;
    QSlider* rotZSlider;
};

#endif // TRANSFORM3DPROPERTIESWIDGET_H
