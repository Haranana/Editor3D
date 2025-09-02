#include "UI/Transform3DPropertiesWidget.h"
#include "UI/Vector3PropertiesWidget.h"
#include "Math/Utility.h"
#include <QFormLayout>

Transform3DPropertiesWidget::Transform3DPropertiesWidget(QWidget* parent, bool showPos, bool showScale, bool showRot) :
    ObjectPropertiesWidget(parent), showPos(showPos), showScale(showScale) , showRot(showRot){
    auto layout = new QFormLayout(this);

    if(showPos){
        posWidget = new Vector3PropertiesWidget(this, posRangeMin, posRangeMax, posSpinStep, 1.0);
        connect(posWidget, &Vector3PropertiesWidget::valueChanged, this, &Transform3DPropertiesWidget::onPosChanged);
        layout->addRow("Pos", posWidget);
    }

    if(showScale){
        scaleWidget = new Vector3PropertiesWidget(this, scaleRangeMin, scaleRangeMax, scaleSpinStep, scaleSliderFactor);
        connect(scaleWidget, &Vector3PropertiesWidget::valueChanged, this, &Transform3DPropertiesWidget::onScaleChanged);
        layout->addRow("Scale", scaleWidget);
    }

    if(showRot){
        rotWidget = new Vector3PropertiesWidget(this, rotRangeMin, rotRangeMax, rotSpinStep, 1.0);
        rotWidget->setValueToDisplay([](double v){ return MathUt::radianToDegree(v); });
        rotWidget->setDisplayToValue([](double v){ return MathUt::degreeToRadian(v); });
        connect(rotWidget, &Vector3PropertiesWidget::valueChanged, this, &Transform3DPropertiesWidget::onRotChanged);
        layout->addRow("Rot", rotWidget);
    }

    setLayout(layout);
}

void Transform3DPropertiesWidget::setObject(std::shared_ptr<Object3D> object){
    obj = object;

    if(showPos){
        posProxy = obj->transform.getPosition();
        posWidget->setVector(posProxy);
    }

    if(showScale){
        scaleProxy = obj->transform.getScales();
        scaleWidget->setVector(scaleProxy);
    }

    if(showRot){
        rotProxy = obj->transform.getAngles();
        rotWidget->setVector(rotProxy);
    }

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
