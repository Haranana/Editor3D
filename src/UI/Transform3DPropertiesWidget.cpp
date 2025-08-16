#include "UI/Transform3DPropertiesWidget.h"
#include "UI/Vector3PropertiesWidget.h"
#include "Math/Utility.h"
#include <QFormLayout>

Transform3DPropertiesWidget::Transform3DPropertiesWidget(QWidget* parent) : ObjectPropertiesWidget(parent){
    auto layout = new QFormLayout(this);

    posWidget = new Vector3PropertiesWidget(this, posRangeMin, posRangeMax, posSpinStep, 1.0);
    scaleWidget = new Vector3PropertiesWidget(this, scaleRangeMin, scaleRangeMax, scaleSpinStep, scaleSliderFactor);
    rotWidget = new Vector3PropertiesWidget(this, rotRangeMin, rotRangeMax, rotSpinStep, 1.0);

    rotWidget->setValueToDisplay([](double v){ return MathUt::radianToDegree(v); });
    rotWidget->setDisplayToValue([](double v){ return MathUt::degreeToRadian(v); });

    connect(posWidget, &Vector3PropertiesWidget::valueChanged, this, &Transform3DPropertiesWidget::onPosChanged);
    connect(scaleWidget, &Vector3PropertiesWidget::valueChanged, this, &Transform3DPropertiesWidget::onScaleChanged);
    connect(rotWidget, &Vector3PropertiesWidget::valueChanged, this, &Transform3DPropertiesWidget::onRotChanged);

    layout->addRow("Pos", posWidget);
    layout->addRow("Scale", scaleWidget);
    layout->addRow("Rot", rotWidget);

    setLayout(layout);
}

void Transform3DPropertiesWidget::setObject(std::shared_ptr<Object3D> object){
    obj = object;

    posProxy = obj->transform.getPosition();
    scaleProxy = obj->transform.getScales();
    rotProxy = obj->transform.getAngles();

    posWidget->setVector(posProxy);
    scaleWidget->setVector(scaleProxy);
    rotWidget->setVector(rotProxy);
}

void Transform3DPropertiesWidget::onPosChanged(){
    if(!obj) return;
    obj->transform.setPosition(posProxy);
    emit objectChanged();
}

void Transform3DPropertiesWidget::onScaleChanged(){
    if(!obj) return;
    obj->transform.setScales(scaleProxy);
    emit objectChanged();
}

void Transform3DPropertiesWidget::onRotChanged(){
    if(!obj) return;
    obj->transform.setAngles(rotProxy);
    emit objectChanged();
}
