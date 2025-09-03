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

class DisplaySettingsPropertiesWidget : public ObjectPropertiesWidget{
    Q_OBJECT
public:
    DisplaySettingsPropertiesWidget(QWidget* parent = nullptr);
    void setObject(std::shared_ptr<Object3D> object) override;
private slots:

    void onRenderModeChanged();
    void onShadingChanged();
    void onDiffuseModelChanged();
    void onSpecularModelChanged();
    void onColorWireframesChanged();

private:
    std::shared_ptr<RenderableObject3D> obj;

    QComboBox* objectRenderModeComboBox;
    QComboBox* objectShadingComboBox;
    QComboBox* objectDiffuseModelComboBox;
    QComboBox* objectSpecularModelComboBox;
    QCheckBox* colorWireframesCheckBox;
};

#endif // DISPLAYSETTINGSPROPERTIESWIDGET_H
