#ifndef MATRIX4_H
#define MATRIX4_H


#include "Vector4.h"
#include <array>
#include <stdexcept>
#include "Math/Vectors.h"
#include <iostream>

class Matrix4 {
public:

    Matrix4();
    Matrix4(std::array<std::array<double,4>,4> matrix);
    Matrix4(const Matrix4& other);

    double get(int row, int col) const;
    Vector4 getCol(int col) const;
    Vector4 getRow(int row) const;
    void set(int row, int col, double val);
    void set(std::array<std::array<double,4>,4> matrix);

    Matrix4 getInversion() const;

    Vector4 operator*(const Vector4& vector) const;

    Matrix4 operator+(const Matrix4& otherMatrix) const;
    Matrix4 operator-(const Matrix4& otherMatrix) const;
    Matrix4 operator*(const Matrix4& otherMatrix) const;

    Matrix4& operator=(const Matrix4& otherMatrix);
    bool operator==(const Matrix4& otherMatrix) const;

    friend std::ostream& operator<<(std::ostream& os, const Matrix4& matrix);

private: //to chyba jest bledne
    std::array<std::array<double,4>,4> matrix = {{
        {{0, 0, 0}},
        {{0, 0, 0}},
        {{0, 0, 0}},
        {{0, 0, 0}}
    }};
};

#endif // MATRIX4_H
