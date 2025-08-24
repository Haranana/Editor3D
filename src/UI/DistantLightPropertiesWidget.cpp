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

    dirProxy = light->direction;
    dirWidget->setVector(dirProxy);
}

void DistantLightPropertiesWidget::onDirectionChanged()
{
    if (!light) return;
    Vector3 d = dirProxy;
    if (d.length() > 0.0001)
        light->direction = d.normalize() * (-1.0);
    emit objectChanged();
}
