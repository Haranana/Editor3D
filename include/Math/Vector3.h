#ifndef VECTOR3_H
#define VECTOR3_H

#include <iostream>

class Vector3{

public:
    Vector3();
    Vector3(double x, double y, double z);
    Vector3(const Vector3& otherVector);

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    bool operator==(const Vector3& otherVector) const;
    Vector3& operator=(const Vector3& otherVector);

    Vector3 operator+(const Vector3& vector)const;
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
