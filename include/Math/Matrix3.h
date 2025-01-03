#ifndef MATRIX3_H
#define MATRIX3_H

#include "Vector3.h"
#include <iostream>
#include <array>
#include <stdexcept>

class Matrix3{
public:
    Matrix3();
    Matrix3(std::array<std::array<double ,3>,3> matrix);
    Matrix3(const Matrix3& otherMatrix);

    double get(int row, int col) const;
    void set(int row, int col, double val);
    void set(std::array<std::array<double ,3>,3> matrix);

    Matrix3 getInversion() const;

    Vector3 operator*(const Vector3& vector) const;
    Matrix3 operator+(const Matrix3& otherMatrix);
    Matrix3 operator-(const Matrix3& otherMatrix);
    Matrix3 operator*(const Matrix3& otherMatrix);

    Matrix3& operator=(const Matrix3& otherMatrix);
    bool operator==(const Matrix3& otherMatrix) const;

    friend std::ostream& operator<<(std::ostream& os, const Matrix3& matrix);


private:
    std::array<std::array<double ,3>,3> matrix = {{
        {{0, 0, 0}},
        {{0, 0, 0}},
        {{0, 0, 0}}
    }};
};


#endif // MATRIX3_H
