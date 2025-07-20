#ifndef VECTOR3_H
#define VECTOR3_H

#include <iostream>
#include <cmath>

class Vector3{

public:
    constexpr Vector3() : x(0), y(0), z(0) {}
    constexpr Vector3(double x, double y, double z) : x(x), y(y), z(z) {}
    constexpr Vector3(const Vector3& otherVector) : x(otherVector.x), y(otherVector.y), z(otherVector.z) {}

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vector3 normalize() const;
    double length() const;
    Vector3 crossProduct(const Vector3& otherVector) const;
    double dotProduct(const Vector3& otherVector) const;

    bool isParallel(const Vector3& otherVector) const;
    bool operator==(const Vector3& otherVector) const;
    Vector3& operator=(const Vector3& otherVector);

    Vector3 operator+(const Vector3& vector)const; //returns cross product beetwen 2 vectors
    Vector3 operator-(const Vector3& vector)const;
    Vector3 operator*(const Vector3& vector)const;
    Vector3 operator/(const Vector3& vector)const;

    Vector3 operator*(double val)const;
    Vector3 operator+(double val)const;
    Vector3 operator-(double val)const;
    Vector3 operator/(double val)const;

    friend std::ostream& operator<<(std::ostream& os, const Vector3& vector);

};

#endif // VECTOR3_H
