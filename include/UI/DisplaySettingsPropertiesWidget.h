#ifndef DISPLAYSETTINGSPROPERTIESWIDGET_H
#define DISPLAYSETTINGSPROPERTIESWIDGET_H
#include "Scene/RenderableObject3D.h"
#include "ObjectPropertiesWidget.h"
#include <QDoubleSpinBox>
#include "ColorPicker.h"
#include <QFormLayout>
#include <QComboBox>
#include "Math/Utility.h"
#include <QCheckBox>
#include "UI/Vector3PropertiesWidget.h"

class DisplaySettingsPropertiesWidget : public ObjectPropertiesWidget{
    Q_OBJECT
public:
    DisplaySettingsPropertiesWidget(QWidget* parent = nullptr);
    void setObject(std::shared_ptr<Object3D> object) override;
private slots:
    void onBaseColorChanged();
    void onRenderModeChanged();
    void onShadingChanged();
    void onDiffuseModelChanged();
    void onSpecularModelChanged();
    void onBackFaceCullingChanged();
    void onColorWireframesChanged();

private:
    std::shared_ptr<RenderableObject3D> obj;

    Vector3 baseColorProxy;
    Vector3PropertiesWidget* baseColorWidget;
    QComboBox* objectRenderModeComboBox;
    QComboBox* objectShadingComboBox;
    QComboBox* objectDiffuseModelComboBox;
    QComboBox* objectSpecularModelComboBox;
    QCheckBox* backFaceCullingCheckBox;
    QCheckBox* colorWireframesCheckBox;
};

#endif // DISPLAYSETTINGSPROPERTIESWIDGET_H
