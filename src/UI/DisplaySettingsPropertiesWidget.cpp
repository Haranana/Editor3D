#include "UI/DisplaySettingsPropertiesWidget.h"

DisplaySettingsPropertiesWidget::DisplaySettingsPropertiesWidget(QWidget* parent) : ObjectPropertiesWidget(parent){

    auto layout = new QFormLayout(this);

    objectRenderModeComboBox = new QComboBox(this);
    objectRenderModeComboBox->addItem("None");
    objectRenderModeComboBox->addItem(QString("Wireframe"));
    objectRenderModeComboBox->addItem(QString("Raster Color"));
    objectRenderModeComboBox->addItem(QString("Raster Texture"));
    layout->addRow("Render mode",objectRenderModeComboBox);
    connect(objectRenderModeComboBox, &QComboBox::currentIndexChanged, this, &DisplaySettingsPropertiesWidget::onRenderModeChanged);

    objectShadingComboBox = new QComboBox(this);
    objectShadingComboBox->addItem(QString("Flat"));
    objectShadingComboBox->addItem(QString("Gouraud"));
    objectShadingComboBox->addItem(QString("Phong"));
    layout->addRow("Shading",objectShadingComboBox);
    connect(objectShadingComboBox, &QComboBox::currentIndexChanged, this, &DisplaySettingsPropertiesWidget::onShadingChanged);

    objectLightingModelComboBox = new QComboBox(this);
    objectLightingModelComboBox->addItem(QString("Unlit"));
    objectLightingModelComboBox->addItem(QString("Lambert"));
    objectLightingModelComboBox->addItem(QString("Phong"));
    objectLightingModelComboBox->addItem(QString("Blinn Phong"));
    objectLightingModelComboBox->addItem(QString("Cook Torrance"));
    layout->addRow("Lighting model",objectLightingModelComboBox);
    connect(objectLightingModelComboBox, &QComboBox::currentIndexChanged, this, &DisplaySettingsPropertiesWidget::onlightingModelChanged);

    colorWireframesCheckBox = new QCheckBox(this);
    connect(colorWireframesCheckBox, &QCheckBox::checkStateChanged, this,
            &DisplaySettingsPropertiesWidget::onColorWireframesChanged);
    layout->addRow("Color wireframe", colorWireframesCheckBox);

    setLayout(layout);
}

void DisplaySettingsPropertiesWidget::setObject(std::shared_ptr<Object3D> object){
    obj = std::dynamic_pointer_cast<RenderableObject3D>(object);

    switch(obj->displaySettings->renderMode){
    case DisplaySettings::RenderMode::NONE:
        objectRenderModeComboBox->setCurrentIndex(0);
        break;
    case DisplaySettings::RenderMode::WIREFRAME:
        objectRenderModeComboBox->setCurrentIndex(1);
        break;
    case DisplaySettings::RenderMode::RASTER_COLOR:
        objectRenderModeComboBox->setCurrentIndex(2);
        break;
    case DisplaySettings::RenderMode::RASTER_TEXTURE:
        objectRenderModeComboBox->setCurrentIndex(3);
        break;
    }

    switch(obj->displaySettings->shadingMode){
    case DisplaySettings::Shading::FLAT:
        objectShadingComboBox->setCurrentIndex(0);
        break;
    case DisplaySettings::Shading::GOURAUD:
        objectShadingComboBox->setCurrentIndex(1);
        break;
    case DisplaySettings::Shading::PHONG:
        objectShadingComboBox->setCurrentIndex(2);
        break;
    }

    switch(obj->displaySettings->lightingMode){
    case DisplaySettings::LightingModel::UNLIT:
        objectLightingModelComboBox->setCurrentIndex(0);
        break;
    case DisplaySettings::LightingModel::LAMBERT:
        objectLightingModelComboBox->setCurrentIndex(1);
        break;
    case DisplaySettings::LightingModel::PHONG:
        objectLightingModelComboBox->setCurrentIndex(2);
        break;
    case DisplaySettings::LightingModel::BLINN_PHONG:
        objectLightingModelComboBox->setCurrentIndex(3);
        break;
    case DisplaySettings::LightingModel::COOK_TORRANCE:
        objectLightingModelComboBox->setCurrentIndex(4);
        break;
    }

    colorWireframesCheckBox->setChecked(obj->displaySettings->colorWireframes);
}

void DisplaySettingsPropertiesWidget::onRenderModeChanged(){
    switch (objectRenderModeComboBox->currentIndex()) {
    case 0:
        obj->displaySettings->renderMode = DisplaySettings::RenderMode::NONE;
        break;
    case 1:
        obj->displaySettings->renderMode = DisplaySettings::RenderMode::WIREFRAME;
        break;
    case 2:
        obj->displaySettings->renderMode = DisplaySettings::RenderMode::RASTER_COLOR;
        break;
    case 3:
        obj->displaySettings->renderMode = DisplaySettings::RenderMode::RASTER_TEXTURE;
        break;
    default:
        obj->displaySettings->renderMode = DisplaySettings::RenderMode::RASTER_COLOR;
        break;
    }
    emit objectChanged();
}



void DisplaySettingsPropertiesWidget::onShadingChanged(){
    switch (objectShadingComboBox->currentIndex()) {
    case 0:
        obj->displaySettings->shadingMode = DisplaySettings::Shading::FLAT;
        break;
    case 1:
        obj->displaySettings->shadingMode = DisplaySettings::Shading::GOURAUD;
        break;
    case 2:
        obj->displaySettings->shadingMode = DisplaySettings::Shading::PHONG;
        break;
    default:
        obj->displaySettings->shadingMode = DisplaySettings::Shading::PHONG;
        break;
    }
    emit objectChanged();
}

void DisplaySettingsPropertiesWidget::onlightingModelChanged(){
    switch (objectLightingModelComboBox->currentIndex()) {
    case 0:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::UNLIT;
        break;
    case 1:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::LAMBERT;
        break;
    case 2:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::PHONG;
        break;
    case 3:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::BLINN_PHONG;
        break;
    case 4:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::COOK_TORRANCE;
        break;
    default:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::LAMBERT;
        break;
    }
    emit objectChanged();
}

void DisplaySettingsPropertiesWidget::onColorWireframesChanged(){
    obj->displaySettings->colorWireframes = colorWireframesCheckBox->isChecked();
    emit objectChanged();
}
