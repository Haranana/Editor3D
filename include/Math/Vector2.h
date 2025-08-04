#ifndef VECTOR2_H
#define VECTOR2_H

#include <iostream>

class Vector2{

public:
    constexpr Vector2() {}
    constexpr Vector2(double x, double y) : x(x) , y(y) {}
    constexpr Vector2(const Vector2& otherVector) : x(otherVector.x) , y(otherVector.y) {}

    double x = 0.0;
    double y = 0.0;

    bool operator==(const Vector2& otherVector) const;
    Vector2& operator=(const Vector2& otherVector);

    Vector2 operator+(const Vector2& vector) const;
    Vector2 operator-(const Vector2& vector) const;
    Vector2 operator*(const Vector2& vector) const;
    Vector2 operator/(const Vector2& vector) const;

    Vector2 operator*(double val) const;
    Vector2 operator+(double val) const;
    Vector2 operator-(double val) const;
    Vector2 operator/(double val) const;

    friend std::ostream& operator<<(std::ostream& os, const Vector2& vector);

};

#endif // VECTOR2_H
