#ifndef RENDERABLEOBJECTPROPERTIESWIDGET_H
#define RENDERABLEOBJECTPROPERTIESWIDGET_H
#include "Scene/RenderableObject3D.h"
#include "ObjectPropertiesWidget.h"
#include <QDoubleSpinBox>
#include "ColorPicker.h"
#include <QFormLayout>
#include <QComboBox>
#include "Math/Utility.h"
#include "UI/Transform3DPropertiesWidget.h"
#include "UI/DisplaySettingsPropertiesWidget.h"
#include <QCheckBox>
#include "UI/MaterialPropertiesWidget.h"

class RenderableObjectPropertiesWidget : public ObjectPropertiesWidget{
    Q_OBJECT
public:
    RenderableObjectPropertiesWidget(QWidget* parent = nullptr);

    void setObject(std::shared_ptr<Object3D> object) override;
private slots:

   // void onColorChanged(const Color& color);
private:

    std::shared_ptr<RenderableObject3D> obj;

    Transform3DPropertiesWidget* transform3DPropertiesWidget;
    DisplaySettingsPropertiesWidget* displaySettingsPropertiesWidget;
    MaterialPropertiesWidget* materialPropertiesWidget;
    //ColorPicker* colorPicker;
};

#endif // RENDERABLEOBJECTPROPERTIESWIDGET_H
