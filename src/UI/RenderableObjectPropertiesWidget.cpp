#include "UI/RenderableObjectPropertiesWidget.h"

RenderableObjectPropertiesWidget::RenderableObjectPropertiesWidget(QWidget* parent) : ObjectPropertiesWidget(parent){
    auto layout = new QFormLayout(this);

    transform3DPropertiesWidget = new Transform3DPropertiesWidget(this);
    layout->addRow(transform3DPropertiesWidget);

    displaySettingsPropertiesWidget = new DisplaySettingsPropertiesWidget(this);
    layout->addRow(displaySettingsPropertiesWidget);

    colorPicker = new ColorPicker(this);
    layout->addRow("Color", colorPicker);
    connect(colorPicker, &ColorPicker::colorChanged, this, &RenderableObjectPropertiesWidget::onColorChanged);
}

void RenderableObjectPropertiesWidget::setObject(std::shared_ptr<Object3D> object){
    obj = std::dynamic_pointer_cast<RenderableObject3D>(object);

    transform3DPropertiesWidget->setObject(object);
    displaySettingsPropertiesWidget->setObject(object);
    colorPicker->setColor(obj->viewportDisplay.color);
}


void RenderableObjectPropertiesWidget::onColorChanged(const Color& color){
    obj->viewportDisplay.color = color;
    emit objectChanged();
}

