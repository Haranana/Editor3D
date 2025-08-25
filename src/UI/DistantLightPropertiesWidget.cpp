#include "UI/DistantLightPropertiesWidget.h"
#include "UI/Vector3PropertiesWidget.h"
#include "UI/LightPropertiesWidget.h"
#include <QFormLayout>

DistantLightPropertiesWidget::DistantLightPropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    lightPropertiesWidget = new LightPropertiesWidget();
    layout->addRow(lightPropertiesWidget);

    auto dynamicBiasRow = new QWidget(this);
    auto dynamicBiasCheckBoxLayout = new QHBoxLayout(dynamicBiasRow);
    dynamicBiasCheckBox = new QCheckBox(this);
    dynamicBiasCheckBoxLayout->addWidget(dynamicBiasCheckBox);
    layout->addRow("Dynamic bias: ", dynamicBiasRow);
    connect(dynamicBiasCheckBox, QOverload<bool>::of(&QCheckBox::clicked), [this](){
        onDynamicBiasChanged();
    });

    dirWidget = new Vector3PropertiesWidget(this, dirMin, dirMax, dirStep, 500.0);
    connect(dirWidget, &Vector3PropertiesWidget::valueChanged, this, &DistantLightPropertiesWidget::onDirectionChanged);
    layout->addRow("Dir", dirWidget);

    setLayout(layout);
}



void DistantLightPropertiesWidget::setObject(std::shared_ptr<Object3D> object)
{
    light = std::dynamic_pointer_cast<DistantLight>(object);
    if (!light) return;
    lightPropertiesWidget->setObject(light);
    if(light->biasType == Light::BiasType::CONSTANT){
        dynamicBiasCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }else{
        dynamicBiasCheckBox->setCheckState(Qt::CheckState::Checked);
    }

    dirProxy = light->direction;
    dirWidget->setVector(dirProxy);
}

void DistantLightPropertiesWidget::onDynamicBiasChanged(){
    if(!light) return;
    if(dynamicBiasCheckBox->checkState() == Qt::CheckState::Checked){
        light->biasType = Light::BiasType::SLOPE_SCALED;
    }else{
        light->biasType = Light::BiasType::CONSTANT;
    }
    emit objectChanged();
}

void DistantLightPropertiesWidget::onDirectionChanged()
{
    if (!light) return;
    Vector3 d = dirProxy;
    if (d.length() > 0.0001)
        light->direction = d.normalize();
    emit objectChanged();
}
