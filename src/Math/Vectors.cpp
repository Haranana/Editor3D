#include "Math/Vectors.h"
#include "algorithm"

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

Vector3 colorToVector3(const Color& color){
    double normB = std::clamp((color.B - 0.0) / (255.0 - 0.0), 0.0, 1.0);
    double normG = std::clamp((color.G - 0.0) / (255.0 - 0.0), 0.0, 1.0);
    double normR = std::clamp((color.R - 0.0) / (255.0 - 0.0), 0.0, 1.0);

    return Vector3(normB , normG, normR);
}

Color vector3ToColor(const Vector3& vector){
    return Color(vector.x*255, vector.y*255, vector.z*255);
}

QColor vector3ToQColor(const Vector3& vector){
    return QColor(vector.z*255, vector.y*255, vector.x*255);
}
}
