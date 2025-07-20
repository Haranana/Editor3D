#include "Math/Matrix4.h"

Matrix4::Matrix4() {}

Matrix4::Matrix4(std::array<std::array<double,4>,4> matrix): matrix(matrix){}

Matrix4::Matrix4(const Matrix4& other) : matrix(other.matrix){}

double Matrix4::get(int row, int col) const {
    return matrix[row][col];
}

Vector4 Matrix4::getCol(int col) const{
    return Vector4(matrix[0][col], matrix[1][col], matrix[2][col], matrix[3][col]);
}
Vector4 Matrix4::getRow(int row) const{
    return Vector4(matrix[row][0], matrix[row][1], matrix[row][2], matrix[row][3]);
}

void Matrix4::set(int row, int col, double val) {
    matrix[row][col] = val;
}

void Matrix4::set(std::array<std::array<double,4>,4> matrix) {
    this->matrix = matrix;
}

Matrix4 Matrix4::getInversion() const
{
    std::array<std::array<double,4>,4> mat = this->matrix;
    std::array<std::array<double,4>,4> inv;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            inv[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }

    for (int i = 0; i < 4; i++) {
        double pivot = mat[i][i];
        if (pivot == 0.0) {
            int swapRow = i+1;
            while (swapRow < 4 && mat[swapRow][i] == 0.0) {
                swapRow++;
            }
            if (swapRow == 4) {
                throw std::invalid_argument("Matrix4 is not invertible (pivot == 0).");
            } else {
                for (int col = 0; col < 4; col++) {
                    std::swap(mat[i][col], mat[swapRow][col]);
                    std::swap(inv[i][col], inv[swapRow][col]);
                }
                pivot = mat[i][i];
            }
        }

        double pivotInv = 1.0 / pivot;
        for (int col = 0; col < 4; col++) {
            mat[i][col] *= pivotInv;
            inv[i][col] *= pivotInv;
        }

        for (int row = 0; row < 4; row++) {
            if (row != i) {
                double factor = mat[row][i];
                for (int col = 0; col < 4; col++) {
                    mat[row][col] -= factor * mat[i][col];
                    inv[row][col] -= factor * inv[i][col];
                }
            }
        }
    }

    Matrix4 result;
    result.set(inv);
    return result;
}

Vector4 Matrix4::operator*(const Vector4& vector) const
{
    Vector4 result;
    result.x = matrix[0][0]*vector.x + matrix[0][1]*vector.y + matrix[0][2]*vector.z + matrix[0][3]*vector.w;
    result.y = matrix[1][0]*vector.x + matrix[1][1]*vector.y + matrix[1][2]*vector.z + matrix[1][3]*vector.w;
    result.z = matrix[2][0]*vector.x + matrix[2][1]*vector.y + matrix[2][2]*vector.z + matrix[2][3]*vector.w;
    result.w = matrix[3][0]*vector.x + matrix[3][1]*vector.y + matrix[3][2]*vector.z + matrix[3][3]*vector.w;

    return result;
}

Matrix4 Matrix4::operator+(const Matrix4& otherMatrix) const
{
    Matrix4 result;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            result.matrix[i][j] = this->matrix[i][j] + otherMatrix.matrix[i][j];
        }
    }
    return result;
}

Matrix4 Matrix4::operator-(const Matrix4& otherMatrix) const
{
    Matrix4 result;
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            result.matrix[i][j] = this->matrix[i][j] - otherMatrix.matrix[i][j];
        }
    }
    return result;
}

Matrix4 Matrix4::operator*(const Matrix4& otherMatrix) const
{
    Matrix4 result;

    for (int rowIt = 0; rowIt < 4; rowIt++) {
        for (int colIt = 0; colIt < 4; colIt++) {
            double elementVal = 0.0;

            for (int tempIt = 0; tempIt < 4; tempIt++) {
                elementVal += matrix[rowIt][tempIt] * otherMatrix.matrix[tempIt][colIt];
            }

            result.matrix[rowIt][colIt] = elementVal;
        }
    }

    return result;
}

Matrix4& Matrix4::operator=(const Matrix4& otherMatrix)
{
    if(this == &otherMatrix){
        return *this;
    }
    this->matrix = otherMatrix.matrix;
    return *this;
}

bool Matrix4::operator==(const Matrix4& other) const
{
    return this->matrix == other.matrix;
}

std::ostream& operator<<(std::ostream& os, const Matrix4& matrix)
{
    for(int i=0; i<4; i++){
        os << "[";
        for(int j=0; j<4; j++){
            os << " " << matrix.matrix[i][j];
        }
        os << " ]" << std::endl;
    }
    return os;
}
