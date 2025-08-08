#include "UI/DisplaySettingsPropertiesWidget.h".h"

DisplaySettingsPropertiesWidget::DisplaySettingsPropertiesWidget(QWidget* parent) : ObjectPropertiesWidget(parent){

    auto layout = new QFormLayout(this);

    objectRenderModeComboBox = new QComboBox(this);
    objectRenderModeComboBox->addItem("None");
    objectRenderModeComboBox->addItem(QString("Raster"));
    objectRenderModeComboBox->addItem(QString("Wireframe"));
    layout->addRow("Render mode",objectRenderModeComboBox);
    connect(objectRenderModeComboBox, &QComboBox::currentIndexChanged, this, &DisplaySettingsPropertiesWidget::onRenderModeChanged);

    objectRasterModeComboBox = new QComboBox(this);
    objectRasterModeComboBox->addItem(QString("None"));
    objectRasterModeComboBox->addItem(QString("Color"));
    objectRasterModeComboBox->addItem(QString("Texture"));
    layout->addRow("Raster mode",objectRasterModeComboBox);
    connect(objectRasterModeComboBox, &QComboBox::currentIndexChanged, this, &DisplaySettingsPropertiesWidget::onRasterModeChanged);

    objectShadingComboBox = new QComboBox(this);
    objectShadingComboBox->addItem(QString("None"));
    objectShadingComboBox->addItem(QString("Flat"));
    objectShadingComboBox->addItem(QString("Gouraud"));
    objectShadingComboBox->addItem(QString("Phong"));
    layout->addRow("Shading",objectShadingComboBox);
    connect(objectShadingComboBox, &QComboBox::currentIndexChanged, this, &DisplaySettingsPropertiesWidget::onShadingChanged);

    objectLightingModelComboBox = new QComboBox(this);
    objectLightingModelComboBox->addItem(QString("None"));
    objectLightingModelComboBox->addItem(QString("Face ratio"));
    objectLightingModelComboBox->addItem(QString("Lambert"));
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
    case DisplaySettings::RenderMode::RASTER:
        objectRenderModeComboBox->setCurrentIndex(1);
        break;
    case DisplaySettings::RenderMode::WIREFRAME:
        objectRenderModeComboBox->setCurrentIndex(2);
        break;
    }

    switch(obj->displaySettings->rasterMode){
    case DisplaySettings::RasterMode::NONE:
        objectRasterModeComboBox->setCurrentIndex(0);
        break;
    case DisplaySettings::RasterMode::COLOR:
        objectRasterModeComboBox->setCurrentIndex(1);
        break;
    case DisplaySettings::RasterMode::TEXTURE:
        objectRasterModeComboBox->setCurrentIndex(2);
        break;
    }

    switch(obj->displaySettings->shadingMode){
    case DisplaySettings::Shading::NONE:
        objectShadingComboBox->setCurrentIndex(0);
        break;
    case DisplaySettings::Shading::FLAT:
        objectShadingComboBox->setCurrentIndex(1);
        break;
    case DisplaySettings::Shading::GOURAUD:
        objectShadingComboBox->setCurrentIndex(2);
        break;
    case DisplaySettings::Shading::PHONG:
        objectShadingComboBox->setCurrentIndex(3);
        break;
    }

    switch(obj->displaySettings->lightingMode){
    case DisplaySettings::LightingModel::NONE:
        objectLightingModelComboBox->setCurrentIndex(0);
        break;
    case DisplaySettings::LightingModel::FACE_RATIO:
        objectLightingModelComboBox->setCurrentIndex(1);
        break;
    case DisplaySettings::LightingModel::LAMBERT:
        objectLightingModelComboBox->setCurrentIndex(2);
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
        obj->displaySettings->renderMode = DisplaySettings::RenderMode::RASTER;
        break;
    case 2:
        obj->displaySettings->renderMode = DisplaySettings::RenderMode::WIREFRAME;
        break;
    default:
        obj->displaySettings->renderMode = DisplaySettings::RenderMode::RASTER;
        break;
    }
    emit objectChanged();
}

void DisplaySettingsPropertiesWidget::onRasterModeChanged(){
    switch (objectRasterModeComboBox->currentIndex()) {
    case 0:
        obj->displaySettings->renderMode = DisplaySettings::RenderMode::NONE;
        break;
    case 1:
        obj->displaySettings->rasterMode = DisplaySettings::RasterMode::COLOR;
        break;
    case 2:
        obj->displaySettings->rasterMode = DisplaySettings::RasterMode::TEXTURE;
        break;
    default:
        obj->displaySettings->rasterMode = DisplaySettings::RasterMode::COLOR;
        break;
    }
    emit objectChanged();
}

void DisplaySettingsPropertiesWidget::onShadingChanged(){
    switch (objectShadingComboBox->currentIndex()) {
    case 0:
        obj->displaySettings->shadingMode = DisplaySettings::Shading::NONE;
        break;
    case 1:
        obj->displaySettings->shadingMode = DisplaySettings::Shading::FLAT;
        break;
    case 2:
        obj->displaySettings->shadingMode = DisplaySettings::Shading::GOURAUD;
        break;
    case 3:
        obj->displaySettings->shadingMode = DisplaySettings::Shading::PHONG;
        break;
    default:
        obj->displaySettings->shadingMode = DisplaySettings::Shading::NONE;
        break;
    }
    emit objectChanged();
}

void DisplaySettingsPropertiesWidget::onlightingModelChanged(){
    switch (objectLightingModelComboBox->currentIndex()) {
    case 0:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::NONE;
        break;
    case 1:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::FACE_RATIO;
        break;
    case 2:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::LAMBERT;
        break;
    default:
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::NONE;
        break;
    }
    emit objectChanged();
}

void DisplaySettingsPropertiesWidget::onColorWireframesChanged(){
    obj->displaySettings->colorWireframes = colorWireframesCheckBox->isChecked();
    emit objectChanged();
}
