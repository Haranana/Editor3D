#include "UI/Transform3DPropertiesWidget.h".h"

Transform3DPropertiesWidget::Transform3DPropertiesWidget(QWidget* parent) : ObjectPropertiesWidget(parent){
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
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Transform3DPropertiesWidget::onPosChanged);
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
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Transform3DPropertiesWidget::onScaleChanged);
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
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Transform3DPropertiesWidget::onRotChanged);
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


    setLayout(layout);
}

void Transform3DPropertiesWidget::setObject(std::shared_ptr<Object3D> object){

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

}

void Transform3DPropertiesWidget::onPosChanged(){
    obj->transform.setPosition(Vector3(posX->value(), posY->value(), posZ->value()));
    emit objectChanged();
}

void Transform3DPropertiesWidget::onScaleChanged(){
    obj->transform.setScales(Vector3(scaleX->value(), scaleY->value(), scaleZ->value()));
    emit objectChanged();
}

void Transform3DPropertiesWidget::onRotChanged(){
    double rotXRadians = MathUt::degreeToRadian(rotX->value());
    double rotYRadians = MathUt::degreeToRadian(rotY->value());
    double rotZRadians = MathUt::degreeToRadian(rotZ->value());
    obj->transform.setAngles(Vector3(rotXRadians, rotYRadians, rotZRadians));
    emit objectChanged();
}
