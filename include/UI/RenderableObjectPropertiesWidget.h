#ifndef RENDERABLEOBJECTPROPERTIESWIDGET_H
#define RENDERABLEOBJECTPROPERTIESWIDGET_H
#include "Scene/RenderableObject3D.h"
#include "ObjectPropertiesWidget.h"
#include <QDoubleSpinBox>
#include "ColorPicker.h"
#include <QFormLayout>
#include <QComboBox>

class RenderableObjectPropertiesWidget : public ObjectPropertiesWidget{
    Q_OBJECT
public:
    RenderableObjectPropertiesWidget(QWidget* parent = nullptr);

    void setObject(std::shared_ptr<Object3D> object) override;
private slots:
    void onPosChanged();
    void onScaleChanged();
    void onRotChanged();
    void onRenderModeChanged();
    void onRasterModeChanged();
    void onShadingChanged();
    void onlightingModelChanged();
    void onColorChanged(const Color& color);

private:
    std::shared_ptr<RenderableObject3D> obj;

    QDoubleSpinBox* posX;
    QDoubleSpinBox* posY;
    QDoubleSpinBox* posZ;

    QDoubleSpinBox* scaleX;
    QDoubleSpinBox* scaleY;
    QDoubleSpinBox* scaleZ;

    QDoubleSpinBox* rotX;
    QDoubleSpinBox* rotY;
    QDoubleSpinBox* rotZ;

    ColorPicker* colorPicker;
    QComboBox* objectRenderModeComboBox;
    QComboBox* objectRasterModeComboBox;
    QComboBox* objectShadingComboBox;
    QComboBox* objectLightingModelComboBox;

};

#endif // RENDERABLEOBJECTPROPERTIESWIDGET_H
