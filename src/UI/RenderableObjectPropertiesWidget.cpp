#include "UI/RenderableObjectPropertiesWidget.h"

RenderableObjectPropertiesWidget::RenderableObjectPropertiesWidget(QWidget* parent) : ObjectPropertiesWidget(parent){
    auto layout = new QFormLayout(this);


    posX = new QDoubleSpinBox(this);
    posY = new QDoubleSpinBox(this);
    posZ = new QDoubleSpinBox(this);
    for (auto* box : {posX, posY, posZ}) {
        box->setRange(-1000, 1000);
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RenderableObjectPropertiesWidget::onPosChanged);
    }
    layout->addRow("Pos X", posX);
    layout->addRow("Pos Y", posY);
    layout->addRow("Pos Z", posZ);

    scaleX = new QDoubleSpinBox(this);
    scaleY = new QDoubleSpinBox(this);
    scaleZ = new QDoubleSpinBox(this);
    for (auto* box : {scaleX, scaleY, scaleZ}) {
        box->setRange(0.01, 100);
        box->setSingleStep(0.01);
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RenderableObjectPropertiesWidget::onScaleChanged);
    }
    layout->addRow("Scale X", scaleX);
    layout->addRow("Scale Y", scaleY);
    layout->addRow("Scale Z", scaleZ);

    rotX = new QDoubleSpinBox(this);
    rotY = new QDoubleSpinBox(this);
    rotZ = new QDoubleSpinBox(this);
    for (auto* box : {rotX, rotY, rotZ}) {
        box->setRange(0.00, 2*M_PI);
        box->setSingleStep(0.01);
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RenderableObjectPropertiesWidget::onScaleChanged);
    }
    layout->addRow("rot X", scaleX);
    layout->addRow("rot Y", scaleY);
    layout->addRow("rot Z", scaleZ);

    objectRenderModeComboBox = new QComboBox(this);
    objectRenderModeComboBox->addItem("None");
    objectRenderModeComboBox->addItem(QString("Raster"));
    objectRenderModeComboBox->addItem(QString("Wireframe"));
    layout->addRow(objectRenderModeComboBox);
    connect(objectRenderModeComboBox, &QComboBox::currentIndexChanged, this, &RenderableObjectPropertiesWidget::onRenderModeChanged);

    objectRasterModeComboBox = new QComboBox(this);
    objectRasterModeComboBox->addItem(QString("None"));
    objectRasterModeComboBox->addItem(QString("Color"));
    objectRasterModeComboBox->addItem(QString("Texture"));
    layout->addRow(objectRasterModeComboBox);
    connect(objectRasterModeComboBox, &QComboBox::currentIndexChanged, this, &RenderableObjectPropertiesWidget::onRasterModeChanged);

    objectShadingComboBox = new QComboBox(this);
    objectShadingComboBox->addItem(QString("None"));
    objectShadingComboBox->addItem(QString("Flat"));
    objectShadingComboBox->addItem(QString("Gouraud"));
    objectShadingComboBox->addItem(QString("Phong"));
    layout->addRow(objectShadingComboBox);
    connect(objectShadingComboBox, &QComboBox::currentIndexChanged, this, &RenderableObjectPropertiesWidget::onShadingChanged);

    colorPicker = new ColorPicker(this);

    layout->addRow("Color", colorPicker);
    connect(colorPicker, &ColorPicker::colorChanged, this, &RenderableObjectPropertiesWidget::onColorChanged);

    setLayout(layout);
}

void RenderableObjectPropertiesWidget::setObject(std::shared_ptr<Object3D> object){
    obj = std::dynamic_pointer_cast<RenderableObject3D>(object);


    Vector3 pos = obj->transform.getPosition();
    posX->setValue(pos.x);
    posY->setValue(pos.y);
    posZ->setValue(pos.z);


    Vector3 scale = obj->transform.getScales();
    scaleX->setValue(scale.x);
    scaleY->setValue(scale.y);
    scaleZ->setValue(scale.z);

    Vector3 rot = obj->transform.getAngles();
    std::cout<<"3.5"<<std::endl;
    std::cout<<rot<<std::endl;
    rotX->setValue(rot.x);
    std::cout<<"3.6"<<std::endl;
    rotY->setValue(rot.y);
    rotZ->setValue(rot.z);

    std::cout<<"4"<<std::endl;
    colorPicker->setColor(obj->viewportDisplay.color);
}

void RenderableObjectPropertiesWidget::onPosChanged(){
    obj->transform.setPosition(Vector3(posX->value(), posY->value(), posZ->value()));
    emit objectChanged();
}

void RenderableObjectPropertiesWidget::onScaleChanged(){
    obj->transform.setPosition(Vector3(scaleX->value(), scaleY->value(), scaleZ->value()));
    emit objectChanged();
}

void RenderableObjectPropertiesWidget::onRotChanged(){
    obj->transform.setPosition(Vector3(posX->value(), posY->value(), posZ->value()));
    emit objectChanged();
}

void RenderableObjectPropertiesWidget::onRenderModeChanged(){
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

void RenderableObjectPropertiesWidget::onRasterModeChanged(){
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

void RenderableObjectPropertiesWidget::onShadingChanged(){
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
        obj->displaySettings->shadingMode = DisplaySettings::Shading::PHONG;
        break;
    }
    emit objectChanged();
}

void RenderableObjectPropertiesWidget::onlightingModelChanged(){
    switch (objectShadingComboBox->currentIndex()) {
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
        obj->displaySettings->lightingMode = DisplaySettings::LightingModel::LAMBERT;
        break;
    }
    emit objectChanged();
}

void RenderableObjectPropertiesWidget::onColorChanged(const Color& color){
    emit objectChanged();
}
