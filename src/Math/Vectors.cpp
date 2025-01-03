#include "Math/Vectors.h"

namespace Vectors{

Vector2 vector4to2(const Vector4& vector){
    return Vector2(vector.x , vector.y);
}

Vector3 vector4to3(const Vector4& vector){
    return Vector3(vector.x , vector.y , vector.z);
}

Vector2 vector3to2(const Vector3& vector){
    return Vector2(vector.x , vector.y);
}

Vector4 vector3to4(const Vector3& vector){
    return Vector4(vector.x , vector.y, vector.z, 1.0);
}

Vector3 vector2to3(const Vector2& vector){
    return Vector3(vector.x , vector.y , 1.0);
}

Vector4 vector2to4(const Vector2& vector){
    return Vector4(vector.x , vector.y , 1.0, 1.0);
}

}
