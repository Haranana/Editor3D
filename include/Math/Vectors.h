#ifndef VECTORS_H
#define VECTORS_H

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Vectors{
    Vector2 vector4to2(const Vector4& vector);
    Vector3 vector4to3(const Vector4& vector);

    Vector2 vector3to2(const Vector3& vector);
    Vector4 vector3to4(const Vector3& vector);

    Vector3 vector2to3(const Vector2& vector);
    Vector4 vector2to4(const Vector2& vector);
}

#endif // VECTORS_H
