#ifndef VECTOR4_H
#define VECTOR4_H



#include <iostream>

class Vector4{

public:
    Vector4();
    Vector4(double x, double y, double z, double w);
    Vector4(const Vector4& otherVector);

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double w = 0.0;

    bool operator==(const Vector4& otherVector) const;
    Vector4& operator=(const Vector4& otherVector);

    Vector4 operator+(const Vector4& vector)const;
    Vector4 operator-(const Vector4& vector)const;
    Vector4 operator*(const Vector4& vector)const;
    Vector4 operator/(const Vector4& vector)const;

    Vector4 operator*(double val)const;
    Vector4 operator+(double val)const;
    Vector4 operator-(double val)const;
    Vector4 operator/(double val)const;

    friend std::ostream& operator<<(std::ostream& os, const Vector4& vector);

};

#endif // VECTOR4_H
