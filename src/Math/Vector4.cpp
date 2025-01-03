#include "Math/Vector4.h"

Vector4::Vector4() : x(0), y(0), z(0), w(0) {}

Vector4::Vector4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

Vector4::Vector4(const Vector4& otherVector) : x(otherVector.x), y(otherVector.y), z(otherVector.z), w(otherVector.w) {}

Vector4& Vector4::operator=(const Vector4& otherVector) {
    if (this != &otherVector) {
        this->x = otherVector.x;
        this->y = otherVector.y;
        this->z = otherVector.z;
        this->w = otherVector.w;
    }
    return *this;
}

bool Vector4::operator==(const Vector4& otherVector) const {
    return this->x == otherVector.x && this->y == otherVector.y && this->z == otherVector.z && this->w == otherVector.w;
}

Vector4 Vector4::operator+(const Vector4& otherVector) const{
    return Vector4(this->x + otherVector.x, this->y + otherVector.y, this->z + otherVector.z, this->w + otherVector.w);
}

Vector4 Vector4::operator-(const Vector4& otherVector) const{
    return Vector4(this->x - otherVector.x, this->y - otherVector.y, this->z - otherVector.z, this->w - otherVector.w);
}

Vector4 Vector4::operator*(const Vector4& otherVector) const{
    return Vector4(this->x * otherVector.x, this->y * otherVector.y, this->z * otherVector.z, this->w * otherVector.w);
}

Vector4 Vector4::operator/(const Vector4& otherVector) const{
    return Vector4(this->x / otherVector.x, this->y / otherVector.y, this->z / otherVector.z, this->w / otherVector.w);
}

Vector4 Vector4::operator+(double val) const{
    return Vector4(this->x + val, this->y + val, this->z + val, this->w + val);
}

Vector4 Vector4::operator-(double val) const{
    return Vector4(this->x - val, this->y - val, this->z - val, this->w - val);
}

Vector4 Vector4::operator*(double val) const{
    return Vector4(this->x * val, this->y * val, this->z * val, this->w * val);
}

Vector4 Vector4::operator/(double val) const{
    return Vector4(this->x / val, this->y / val, this->z / val, this->w / val);
}

std::ostream& operator<<(std::ostream& os, const Vector4& vector4) {
    os << "(" << vector4.x << "," << vector4.y << "," << vector4.z << "," << vector4.w << ")";
    return os;
}
