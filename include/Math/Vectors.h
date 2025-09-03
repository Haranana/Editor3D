#ifndef VECTORS_H
#define VECTORS_H

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Rendering/Color.h"
#include <QColor>

namespace Vectors{
    Vector2 vector4to2(const Vector4& vector);
    Vector3 vector4to3(const Vector4& vector);

    Vector2 vector3to2(const Vector3& vector);
    Vector4 vector3to4(const Vector3& vector);

    Vector3 vector2to3(const Vector2& vector);
    Vector4 vector2to4(const Vector2& vector);

    //returns vector based on input Color
    // B -> x
    // G -> y
    // R -> z
    // where B,G,R [0,255] and x,y,z [0.0,1.0]
    Vector3 colorToVector3(const Color& color);

    //returns color based on input Vector
    // x -> B
    // y -> G
    // z -> R
    // where B,G,R [0,255] and x,y,z [0.0,1.0]
    Color vector3ToColor(const Vector3& vector);

    QColor vector3ToQColor(const Vector3& vector);
}

#endif // VECTORS_H
