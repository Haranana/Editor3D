#ifndef LIGHTFILTERINGPROPERTIESWIDGET_H
#define LIGHTFILTERINGPROPERTIESWIDGET_H

#include "UI/ObjectPropertiesWidget.h"
#include "Scene/Light.h"
#include <QFormLayout>
#include <QComboBox>
#include <memory>

class LightFilteringPropertiesWidget : public ObjectPropertiesWidget {
    Q_OBJECT
public:
    LightFilteringPropertiesWidget(QWidget* parent = nullptr);
    void setObject(std::shared_ptr<Object3D> object) override;

private slots:
    void onFilteringChanged(int index);

private:
    int indexFromFiltering(Light::FilteringType type) const;
    Light::FilteringType filteringFromIndex(int id) const;

    std::shared_ptr<Light> light;
    QComboBox* filteringCombo;
};


#endif // LIGHTFILTERINGPROPERTIESWIDGET_H
