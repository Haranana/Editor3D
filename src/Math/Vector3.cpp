#include "Math/Vector3.h"

Vector3::Vector3() : x(0), y(0), z(0) {}

Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

Vector3::Vector3(const Vector3& otherVector) : x(otherVector.x), y(otherVector.y), z(otherVector.z) {}

Vector3& Vector3::operator=(const Vector3& otherVector) {
    if (this != &otherVector) {
        this->x = otherVector.x;
        this->y = otherVector.y;
        this->z = otherVector.z;
    }
    return *this;
}

bool Vector3::operator==(const Vector3& otherVector) const {
    return this->x == otherVector.x && this->y == otherVector.y && this->z == otherVector.z;
}

Vector3 Vector3::operator+(const Vector3& otherVector) const{
    return Vector3(this->x + otherVector.x, this->y + otherVector.y, this->z + otherVector.z);
}

Vector3 Vector3::operator-(const Vector3& otherVector) const{
    return Vector3(this->x - otherVector.x, this->y - otherVector.y, this->z - otherVector.z);
}

Vector3 Vector3::operator*(const Vector3& otherVector) const{
    return Vector3(this->x * otherVector.x, this->y * otherVector.y, this->z * otherVector.z);
}

Vector3 Vector3::operator/(const Vector3& otherVector) const{
    return Vector3(this->x / otherVector.x, this->y / otherVector.y, this->z / otherVector.z);
}

Vector3 Vector3::operator+(double val) const{
    return Vector3(this->x + val, this->y + val, this->z + val);
}

Vector3 Vector3::operator-(double val) const{
    return Vector3(this->x - val, this->y - val, this->z - val);
}

Vector3 Vector3::operator*(double val) const{
    return Vector3(this->x * val, this->y * val, this->z * val);
}

Vector3 Vector3::operator/(double val) const{
    return Vector3(this->x / val, this->y / val, this->z / val);
}

std::ostream& operator<<(std::ostream& os, const Vector3& vector3) {
    os << "(" << vector3.x << "," << vector3.y << "," << vector3.z << ")";
    return os;
}
