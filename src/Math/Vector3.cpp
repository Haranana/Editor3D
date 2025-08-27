#include "Math/Vector3.h"
#include "Math/Utility.h"

Vector3 Vector3::normalize() const{
    return MathUt::equal(this->length(),0.0)? Vector3{0,0,0} : *this/this->length();
}

double Vector3::length() const{
    return sqrt(pow(x,2)+pow(y,2)+pow(z,2));
}

Vector3 Vector3::crossProduct(const Vector3& otherVector) const{
    return Vector3(this->y * otherVector.z - this->z * otherVector.y,
                   this->z * otherVector.x - this->x * otherVector.z,
                   this->x * otherVector.y - this->y * otherVector.x);
}

double Vector3::dotProduct(const Vector3& otherVector) const{
    return this->x * otherVector.x+
                   this->y * otherVector.y+
                   this->z * otherVector.z;
}

Vector3& Vector3::operator=(const Vector3& otherVector) {
    if (this != &otherVector) {
        this->x = otherVector.x;
        this->y = otherVector.y;
        this->z = otherVector.z;
    }
    return *this;
}

bool Vector3::isParallel(const Vector3& otherVector) const {

    static constexpr double accuracy = 0.999;
    return fabs(this->dotProduct(otherVector)) > accuracy;
    /*
    constexpr double epsilon = 1e-6;
    Vector3 cross = this->crossProduct(otherVector);
    return cross.length() < epsilon;*/
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
   // return Vector3(this->x * otherVector.x, this->y * otherVector.y, this->z * otherVector.z);
    return Vector3(this->y * otherVector.z - this->z * otherVector.y,
                  this->z * otherVector.x - this->x * otherVector.z,
                  this->x * otherVector.y - this->y * otherVector.x);
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

QDebug& operator<<(QDebug& qd, const Vector3& vector){
    qd << "(" << vector.x << "," << vector.y << "," << vector.z << ")";
    return qd;
}


