#ifndef PAINTOBJECTMETHODS_H
#define PAINTOBJECTMETHODS_H

#include "Scene/Object3D.h"
#include <memory>
#include <QImage>
/*
 * Collection of method to paint 3D Objects
 */
namespace PaintObjectMethods{

    /*
     * Basic painting consisting of drawing every edge of Object3D with linePainter
     */
    void basic(std::shared_ptr<QImage> img, std::shared_ptr<Object3D> object);
}

#endif // PAINTOBJECTMETHODS_H
