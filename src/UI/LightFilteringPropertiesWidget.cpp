#include "UI/LightFilteringPropertiesWidget.h"
#include <QFormLayout>

LightFilteringPropertiesWidget::LightFilteringPropertiesWidget(QWidget* parent)
    : ObjectPropertiesWidget(parent)
{
    auto layout = new QFormLayout(this);

    filteringCombo = new QComboBox(this);
    filteringCombo->addItem("None");
    filteringCombo->addItem("Bilinear");
    filteringCombo->addItem("PCF 3x3");
    filteringCombo->addItem("PCF 5x5");
    filteringCombo->addItem("PCF Poisson");
    filteringCombo->addItem("PCSS");
    layout->addRow(filteringCombo);

    connect(filteringCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LightFilteringPropertiesWidget::onFilteringChanged);

    setLayout(layout);
}

void LightFilteringPropertiesWidget::setObject(std::shared_ptr<Object3D> object)
{
    light = std::dynamic_pointer_cast<Light>(object);
    if (!light) return;

    filteringCombo->blockSignals(true);
    filteringCombo->setCurrentIndex(indexFromFiltering(light->filteringType));
    filteringCombo->blockSignals(false);
}

void LightFilteringPropertiesWidget::onFilteringChanged(int index)
{
    if (!light) return;
    light->filteringType = filteringFromIndex(index);
    emit objectChanged();
}

int LightFilteringPropertiesWidget::indexFromFiltering(Light::FilteringType type) const
{
    switch (type) {
    case Light::FilteringType::NONE: return 0;
    case Light::FilteringType::BILINEAR: return 1;
    case Light::FilteringType::PCF_3x3: return 2;
    case Light::FilteringType::PCF_5x5: return 3;
    case Light::FilteringType::PCF_POISSON: return 4;
    case Light::FilteringType::PCSS: return 5;
    }
    return 0;
}

Light::FilteringType LightFilteringPropertiesWidget::filteringFromIndex(int index) const
{
    switch (index) {
    case 0: return Light::FilteringType::NONE;
    case 1: return Light::FilteringType::BILINEAR;
    case 2: return Light::FilteringType::PCF_3x3;
    case 3: return Light::FilteringType::PCF_5x5;
    case 4: return Light::FilteringType::PCF_POISSON;
    case 5: return Light::FilteringType::PCSS;
    default: return Light::FilteringType::NONE;
    }
}
