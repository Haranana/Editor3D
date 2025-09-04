#include "UI/DisplaySettingsPropertiesWidget.h"

DisplaySettingsPropertiesWidget::DisplaySettingsPropertiesWidget(QWidget* parent) : ObjectPropertiesWidget(parent){

    auto layout = new QFormLayout(this);

    baseColorWidget = new Vector3PropertiesWidget(this, 0.0, 1.0, 0.01, 100.0, 2, true);
    layout->addRow("Base color",baseColorWidget);
    connect(baseColorWidget, &Vector3PropertiesWidget::valueChanged, this, &DisplaySettingsPropertiesWidget::onBaseColorChanged);

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

    objectDiffuseModelComboBox = new QComboBox(this);
    objectDiffuseModelComboBox->addItem(QString("None"));
    objectDiffuseModelComboBox->addItem(QString("Lambert"));
    layout->addRow("Diffuse model",objectDiffuseModelComboBox);
    connect(objectDiffuseModelComboBox, &QComboBox::currentIndexChanged, this, &DisplaySettingsPropertiesWidget::onDiffuseModelChanged);

    objectSpecularModelComboBox = new QComboBox(this);
    objectSpecularModelComboBox->addItem(QString("None"));
    objectSpecularModelComboBox->addItem(QString("Phong"));
    objectSpecularModelComboBox->addItem(QString("Blinn Phong"));
    objectSpecularModelComboBox->addItem(QString("Cook Torrance"));
    layout->addRow("Specular model",objectSpecularModelComboBox);
    connect(objectSpecularModelComboBox, &QComboBox::currentIndexChanged, this, &DisplaySettingsPropertiesWidget::onSpecularModelChanged);

    backFaceCullingCheckBox = new QCheckBox(this);
    connect(backFaceCullingCheckBox, &QCheckBox::checkStateChanged, this,
            &DisplaySettingsPropertiesWidget::onBackFaceCullingChanged);
    layout->addRow("Back face culling", backFaceCullingCheckBox);

    colorWireframesCheckBox = new QCheckBox(this);
    connect(colorWireframesCheckBox, &QCheckBox::checkStateChanged, this,
            &DisplaySettingsPropertiesWidget::onColorWireframesChanged);
    layout->addRow("show wireframe", colorWireframesCheckBox);

    setLayout(layout);
}

void DisplaySettingsPropertiesWidget::setObject(std::shared_ptr<Object3D> object){
    obj = std::dynamic_pointer_cast<RenderableObject3D>(object);
    baseColorProxy = obj->displaySettings->baseColor;
    baseColorWidget->setVector(baseColorProxy);

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


    switch(obj->displaySettings->diffuseModel){
    case DisplaySettings::DiffuseModel::NONE:
        objectDiffuseModelComboBox->setCurrentIndex(0);
        break;
    case DisplaySettings::DiffuseModel::LAMBERT:
        objectDiffuseModelComboBox->setCurrentIndex(1);
        break;
    }


    switch(obj->displaySettings->specularModel){
    case DisplaySettings::SpecularModel::NONE:
        objectSpecularModelComboBox->setCurrentIndex(0);
        break;
    case DisplaySettings::SpecularModel::PHONG:
        objectSpecularModelComboBox->setCurrentIndex(1);
        break;
    case DisplaySettings::SpecularModel::BLINN_PHONG:
        objectSpecularModelComboBox->setCurrentIndex(2);
        break;
    case DisplaySettings::SpecularModel::COOK_TORRANCE:
        objectSpecularModelComboBox->setCurrentIndex(3);
        break;
    }

    backFaceCullingCheckBox->setChecked(obj->displaySettings->backFaceCulling);

    colorWireframesCheckBox->setChecked(obj->displaySettings->colorWireframes);
}

void DisplaySettingsPropertiesWidget::onBaseColorChanged(){
    if(!obj) return;
    obj->displaySettings->baseColor = RendUt::sRGBToLinear(baseColorProxy);
    emit objectChanged();
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


void DisplaySettingsPropertiesWidget::onDiffuseModelChanged(){
    switch (objectDiffuseModelComboBox->currentIndex()) {
    case 0:
        obj->displaySettings->diffuseModel = DisplaySettings::DiffuseModel::NONE;
        break;
    case 1:
        obj->displaySettings->diffuseModel = DisplaySettings::DiffuseModel::LAMBERT;
        break;
    default:
        obj->displaySettings->diffuseModel = DisplaySettings::DiffuseModel::LAMBERT;
        break;
    }
    emit objectChanged();
}

void DisplaySettingsPropertiesWidget::onSpecularModelChanged(){
    switch (objectSpecularModelComboBox->currentIndex()) {
    case 0:
        obj->displaySettings->specularModel = DisplaySettings::SpecularModel::NONE;
        break;
    case 1:
        obj->displaySettings->specularModel = DisplaySettings::SpecularModel::PHONG;
        break;
    case 2:
        obj->displaySettings->specularModel = DisplaySettings::SpecularModel::BLINN_PHONG;
        break;
    case 3:
        obj->displaySettings->specularModel = DisplaySettings::SpecularModel::COOK_TORRANCE;
        break;
    default:
        obj->displaySettings->diffuseModel = DisplaySettings::DiffuseModel::LAMBERT;
        break;
    }

    emit objectChanged();
}

void DisplaySettingsPropertiesWidget::onColorWireframesChanged(){
    obj->displaySettings->colorWireframes = colorWireframesCheckBox->isChecked();
    emit objectChanged();
}

void DisplaySettingsPropertiesWidget::onBackFaceCullingChanged(){
    obj->displaySettings->backFaceCulling = backFaceCullingCheckBox->isChecked();
    emit objectChanged();
}
