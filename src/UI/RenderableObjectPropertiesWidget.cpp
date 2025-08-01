#include "UI/RenderableObjectPropertiesWidget.h"

RenderableObjectPropertiesWidget::RenderableObjectPropertiesWidget(QWidget* parent) : ObjectPropertiesWidget(parent){
    auto layout = new QFormLayout(this);

    auto posXRow = new QWidget(this);
    auto posXLayout = new QHBoxLayout(posXRow);
    auto posYRow = new QWidget(this);
    auto posYLayout = new QHBoxLayout(posYRow);
    auto posZRow = new QWidget(this);
    auto posZLayout = new QHBoxLayout(posZRow);

    posX = new QDoubleSpinBox(this);
    posX->setRange(posRangeMin, posRangeMax);
    posX->setSingleStep(posSpinStep);
    posY = new QDoubleSpinBox(this);
    posY->setRange(posRangeMin, posRangeMax);
    posY->setSingleStep(posSpinStep);
    posZ = new QDoubleSpinBox(this);
    posZ->setRange(posRangeMin, posRangeMax);
    posZ->setSingleStep(posSpinStep);

    posXSlider = new QSlider(Qt::Horizontal, this);
    posXSlider->setRange(static_cast<int>(posRangeMin), static_cast<int>(posRangeMax));
    posYSlider = new QSlider(Qt::Horizontal, this);
    posYSlider->setRange(static_cast<int>(posRangeMin), static_cast<int>(posRangeMax));
    posZSlider = new QSlider(Qt::Horizontal, this);
    posZSlider->setRange(static_cast<int>(posRangeMin), static_cast<int>(posRangeMax));

    for (auto* box : {posX, posY, posZ}) {
        //box->setRange(-1000, 1000);
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RenderableObjectPropertiesWidget::onPosChanged);
    }

    connect(posX, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v);
                if (posXSlider->value() != iv)
                    posXSlider->setValue(iv);
            });
    connect(posXSlider, &QSlider::valueChanged,
            [this](int v) {
                if (int(posX->value()) != v)
                    posX->setValue(v);
            });

    connect(posY, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v);
                if (posYSlider->value() != iv)
                    posYSlider->setValue(iv);
            });

    connect(posYSlider, &QSlider::valueChanged,
            [this](int v) {
                if (int(posY->value()) != v)
                    posY->setValue(v);
            });

    connect(posZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v);
                if (posZSlider->value() != iv)
                    posZSlider->setValue(iv);
            });
    connect(posZSlider, &QSlider::valueChanged,
            [this](int v) {
                if (int(posZ->value()) != v)
                    posZ->setValue(v);
            });

    posXLayout->addWidget(posX);
    posXLayout->addWidget(posXSlider);
    posYLayout->addWidget(posY);
    posYLayout->addWidget(posYSlider);
    posZLayout->addWidget(posZ);
    posZLayout->addWidget(posZSlider);

    layout->addRow("Pos X",posXRow);
    layout->addRow("Pos Y",posYRow);
    layout->addRow("Pos Z",posZRow);

    auto scaleXRow = new QWidget(this);
    auto scaleXLayout = new QHBoxLayout(scaleXRow);
    auto scaleYRow = new QWidget(this);
    auto scaleYLayout = new QHBoxLayout(scaleYRow);
    auto scaleZRow = new QWidget(this);
    auto scaleZLayout = new QHBoxLayout(scaleZRow);

    scaleX = new QDoubleSpinBox(this);
    scaleX->setRange(scaleRangeMin, scaleRangeMax);
    scaleX->setSingleStep(scaleSpinStep);
    scaleY = new QDoubleSpinBox(this);
    scaleY->setRange(scaleRangeMin, scaleRangeMax);
    scaleY->setSingleStep(scaleSpinStep);
    scaleZ = new QDoubleSpinBox(this);
    scaleZ->setRange(scaleRangeMin, scaleRangeMax);
    scaleZ->setSingleStep(scaleSpinStep);

    scaleXSlider = new QSlider(Qt::Horizontal, this);
    scaleXSlider->setRange(static_cast<int>(scaleRangeMin * scaleSliderFactor),
                           static_cast<int>(scaleRangeMax * scaleSliderFactor));
    scaleYSlider = new QSlider(Qt::Horizontal, this);
    scaleYSlider->setRange(static_cast<int>(scaleRangeMin * scaleSliderFactor),
                           static_cast<int>(scaleRangeMax * scaleSliderFactor));
    scaleZSlider = new QSlider(Qt::Horizontal, this);
    scaleZSlider->setRange(static_cast<int>(scaleRangeMin * scaleSliderFactor),
                           static_cast<int>(scaleRangeMax * scaleSliderFactor));

    for (auto* box : {scaleX, scaleY, scaleZ}) {
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RenderableObjectPropertiesWidget::onScaleChanged);
    }

    connect(scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v * 100);
                if (scaleXSlider->value() != iv)
                    scaleXSlider->setValue(iv);
            });
    connect(scaleXSlider, &QSlider::valueChanged,
            [this](int v) {
                double dv = v / 100.0;
                if (std::abs(scaleX->value() - dv) > 0.0001)
                    scaleX->setValue(dv);
            });

    connect(scaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v * 100);
                if (scaleYSlider->value() != iv)
                    scaleYSlider->setValue(iv);
            });
    connect(scaleYSlider, &QSlider::valueChanged,
            [this](int v) {
                double dv = v / 100.0;
                if (std::abs(scaleY->value() - dv) > 0.0001)
                    scaleY->setValue(dv);
            });

    connect(scaleZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v * 100);
                if (scaleZSlider->value() != iv)
                    scaleZSlider->setValue(iv);
            });
    connect(scaleZSlider, &QSlider::valueChanged,
            [this](int v) {
                double dv = v / 100.0;
                if (std::abs(scaleZ->value() - dv) > 0.0001)
                    scaleZ->setValue(dv);
            });

    scaleXLayout->addWidget(scaleX);
    scaleXLayout->addWidget(scaleXSlider);
    scaleYLayout->addWidget(scaleY);
    scaleYLayout->addWidget(scaleYSlider);
    scaleZLayout->addWidget(scaleZ);
    scaleZLayout->addWidget(scaleZSlider);
    layout->addRow("Scale X", scaleXRow);
    layout->addRow("Scale Y", scaleYRow);
    layout->addRow("Scale Z", scaleZRow);

    auto rotXRow = new QWidget(this);
    auto rotXLayout = new QHBoxLayout(rotXRow);
    auto rotYRow = new QWidget(this);
    auto rotYLayout = new QHBoxLayout(rotYRow);
    auto rotZRow = new QWidget(this);
    auto rotZLayout = new QHBoxLayout(rotZRow);

    rotX = new QDoubleSpinBox(this);
    rotX->setRange(rotRangeMin, rotRangeMax);
    rotX->setSingleStep(rotSpinStep);
    rotX->setDecimals(rotSpinDecimals);
    rotY = new QDoubleSpinBox(this);
    rotY->setRange(rotRangeMin, rotRangeMax);
    rotY->setSingleStep(rotSpinStep);
    rotY->setDecimals(rotSpinDecimals);
    rotZ = new QDoubleSpinBox(this);
    rotZ->setRange(rotRangeMin, rotRangeMax);
    rotZ->setSingleStep(rotSpinStep);
    rotZ->setDecimals(rotSpinDecimals);

    rotXSlider = new QSlider(Qt::Horizontal, this);
    rotXSlider->setRange(static_cast<int>(rotRangeMin), static_cast<int>(rotRangeMax));
    rotYSlider = new QSlider(Qt::Horizontal, this);
    rotYSlider->setRange(static_cast<int>(rotRangeMin), static_cast<int>(rotRangeMax));
    rotZSlider = new QSlider(Qt::Horizontal, this);
    rotZSlider->setRange(static_cast<int>(rotRangeMin), static_cast<int>(rotRangeMax));

    for (auto* box : {rotX, rotY, rotZ}) {
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &RenderableObjectPropertiesWidget::onRotChanged);
    }

    connect(rotX, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v);
                if (rotXSlider->value() != iv)
                    rotXSlider->setValue(iv);
            });
    connect(rotXSlider, &QSlider::valueChanged,
            [this](int v) {
                if (int(rotX->value()) != v)
                    rotX->setValue(v);
            });

    connect(rotY, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v);
                if (rotYSlider->value() != iv)
                    rotYSlider->setValue(iv);
            });
    connect(rotYSlider, &QSlider::valueChanged,
            [this](int v) {
                if (int(rotY->value()) != v)
                    rotY->setValue(v);
            });

    connect(rotZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [this](double v) {
                int iv = int(v);
                if (rotZSlider->value() != iv)
                    rotZSlider->setValue(iv);
            });
    connect(rotZSlider, &QSlider::valueChanged,
            [this](int v) {
                if (int(rotZ->value()) != v)
                    rotZ->setValue(v);
            });

    rotXLayout->addWidget(rotX);
    rotXLayout->addWidget(rotXSlider);
    rotYLayout->addWidget(rotY);
    rotYLayout->addWidget(rotYSlider);
    rotZLayout->addWidget(rotZ);
    rotZLayout->addWidget(rotZSlider);

    layout->addRow("Rot X", rotXRow);
    layout->addRow("Rot Y", rotYRow);
    layout->addRow("Rot Z", rotZRow);

    objectRenderModeComboBox = new QComboBox(this);
    objectRenderModeComboBox->addItem("None");
    objectRenderModeComboBox->addItem(QString("Raster"));
    objectRenderModeComboBox->addItem(QString("Wireframe"));
    layout->addRow("Render mode",objectRenderModeComboBox);
    connect(objectRenderModeComboBox, &QComboBox::currentIndexChanged, this, &RenderableObjectPropertiesWidget::onRenderModeChanged);

    objectRasterModeComboBox = new QComboBox(this);
    objectRasterModeComboBox->addItem(QString("None"));
    objectRasterModeComboBox->addItem(QString("Color"));
    objectRasterModeComboBox->addItem(QString("Texture"));
    layout->addRow("Raster mode",objectRasterModeComboBox);
    connect(objectRasterModeComboBox, &QComboBox::currentIndexChanged, this, &RenderableObjectPropertiesWidget::onRasterModeChanged);

    objectShadingComboBox = new QComboBox(this);
    objectShadingComboBox->addItem(QString("None"));
    objectShadingComboBox->addItem(QString("Flat"));
    objectShadingComboBox->addItem(QString("Gouraud"));
    objectShadingComboBox->addItem(QString("Phong"));
    layout->addRow("Shading",objectShadingComboBox);
    connect(objectShadingComboBox, &QComboBox::currentIndexChanged, this, &RenderableObjectPropertiesWidget::onShadingChanged);

    objectLightingModelComboBox = new QComboBox(this);
    objectLightingModelComboBox->addItem(QString("None"));
    objectLightingModelComboBox->addItem(QString("Face ratio"));
    objectLightingModelComboBox->addItem(QString("Lambert"));
    layout->addRow("Lighting model",objectLightingModelComboBox);
    connect(objectLightingModelComboBox, &QComboBox::currentIndexChanged, this, &RenderableObjectPropertiesWidget::onlightingModelChanged);

    colorPicker = new ColorPicker(this);

    layout->addRow("Color", colorPicker);
    connect(colorPicker, &ColorPicker::colorChanged, this, &RenderableObjectPropertiesWidget::onColorChanged);

    colorWireframesCheckBox = new QCheckBox(this);
    connect(colorWireframesCheckBox, &QCheckBox::checkStateChanged, this,
            &RenderableObjectPropertiesWidget::onColorWireframesChanged);
    layout->addRow("Color wireframe", colorWireframesCheckBox);

    setLayout(layout);
}

void RenderableObjectPropertiesWidget::setObject(std::shared_ptr<Object3D> object){
    obj = std::dynamic_pointer_cast<RenderableObject3D>(object);
    Vector3 pos = obj->transform.getPosition();
    posXSlider->setValue(pos.x);
    posYSlider->setValue(pos.y);
    posZSlider->setValue(pos.z);

    Vector3 scale = obj->transform.getScales();
    scaleX->setValue(scale.x);
    scaleY->setValue(scale.y);
    scaleZ->setValue(scale.z);

    Vector3 rot = obj->transform.getAngles();
    rotX->setValue(rot.x);
    rotY->setValue(rot.y);
    rotZ->setValue(rot.z);

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

    colorPicker->setColor(obj->viewportDisplay.color);

    colorWireframesCheckBox->setChecked(obj->displaySettings->colorWireframes);
}

void RenderableObjectPropertiesWidget::onPosChanged(){
    obj->transform.setPosition(Vector3(posX->value(), posY->value(), posZ->value()));
    emit objectChanged();
}

void RenderableObjectPropertiesWidget::onScaleChanged(){
    obj->transform.setScales(Vector3(scaleX->value(), scaleY->value(), scaleZ->value()));
    emit objectChanged();
}

void RenderableObjectPropertiesWidget::onRotChanged(){
    double rotXRadians = MathUt::degreeToRadian(rotX->value());
    double rotYRadians = MathUt::degreeToRadian(rotY->value());
    double rotZRadians = MathUt::degreeToRadian(rotZ->value());
    obj->transform.setAngles(Vector3(rotXRadians, rotYRadians, rotZRadians));
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
        obj->displaySettings->shadingMode = DisplaySettings::Shading::NONE;
        break;
    }
    emit objectChanged();
}

void RenderableObjectPropertiesWidget::onlightingModelChanged(){
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

void RenderableObjectPropertiesWidget::onColorChanged(const Color& color){
    obj->viewportDisplay.color = color;
    emit objectChanged();
}

void RenderableObjectPropertiesWidget::onColorWireframesChanged(){
    obj->displaySettings->colorWireframes = colorWireframesCheckBox->isChecked();
    emit objectChanged();
}
