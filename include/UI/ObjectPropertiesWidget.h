#ifndef OBJECTPROPERTIESWIDGET_H
#define OBJECTPROPERTIESWIDGET_H
#include <QWidget>
#include "Scene/Object3D.h"

class ObjectPropertiesWidget : public QWidget{
    Q_OBJECT
public:
    ObjectPropertiesWidget(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual void setObject(std::shared_ptr<Object3D> object) = 0;

signals:
    void objectChanged();
};

#endif // OBJECTPROPERTIESWIDGET_H
