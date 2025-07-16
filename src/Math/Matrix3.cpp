#include "Math/Matrix3.h"

Matrix3::Matrix3(){}

Matrix3::Matrix3(std::array<std::array<double ,3>,3> matrix) : matrix(matrix) {}

Matrix3::Matrix3(const Matrix3& otherMatrix) : matrix(otherMatrix.matrix) {}

double Matrix3::get(int row, int col) const{
    return matrix[row][col];
}
void Matrix3::set(int row, int col, double val){
    matrix[row][col] = val;
}

void Matrix3::set(std::array<std::array<double ,3>,3> matrix){
    this->matrix = matrix;
}

Matrix3 Matrix3::getInversion() const{

    double a = matrix[0][0];
    double b = matrix[0][1];
    double c = matrix[0][2];
    double d = matrix[1][0];
    double e = matrix[1][1];
    double f = matrix[1][2];
    double g = matrix[2][0];
    double h = matrix[2][1];
    double i = matrix[2][2];

    double det = a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);

    if (det == 0) {
        throw std::invalid_argument("Matrix is not invertible (determinant is zero).");
    }

    double invDet = 1.0 / det;

    Matrix3 adj;
    adj.set(0, 0,  (e*i - f*h) * invDet);
    adj.set(0, 1, -(b*i - c*h) * invDet);
    adj.set(0, 2,  (b*f - c*e) * invDet);
    adj.set(1, 0, -(d*i - f*g) * invDet);
    adj.set(1, 1,  (a*i - c*g) * invDet);
    adj.set(1, 2, -(a*f - c*d) * invDet);
    adj.set(2, 0,  (d*h - e*g) * invDet);
    adj.set(2, 1, -(a*h - b*g) * invDet);
    adj.set(2, 2,  (a*e - b*d) * invDet);

    return adj;
}

Matrix3 Matrix3::transpose() const{
    Matrix3 resultMatrix;
    for (int rowIt = 0; rowIt < 3; rowIt++) {
        for (int colIt = 0; colIt < 3; colIt++) {
            resultMatrix.matrix[rowIt][colIt] = matrix[colIt][rowIt];
        }
    }
    return resultMatrix;
}

Vector3 Matrix3::operator*(const Vector3& vector) const{
    Vector3 result;

    double elementVal = 0.0;
    elementVal += matrix[0][0] * vector.x;
    elementVal += matrix[0][1] * vector.y;
    elementVal += matrix[0][2] * vector.z;

    result.x = elementVal;

    elementVal = 0.0;
    elementVal += matrix[1][0] * vector.x;
    elementVal += matrix[1][1] * vector.y;
    elementVal += matrix[1][2] * vector.z;

    result.y = elementVal;

    elementVal = 0.0;
    elementVal += matrix[2][0] * vector.x;
    elementVal += matrix[2][1] * vector.y;
    elementVal += matrix[2][2] * vector.z;

    result.z = elementVal;

    return result;
}

Matrix3 Matrix3::operator+(const Matrix3& otherMatrix){
    Matrix3 result;

    for (int rowIt = 0; rowIt < 3; rowIt++) {
        for (int colIt = 0; colIt < 3; colIt++) {
            result.matrix[rowIt][colIt] = this->matrix[rowIt][colIt] + otherMatrix.matrix[rowIt][colIt];
        }
    }

    return result;
}

Matrix3 Matrix3::operator-(const Matrix3& otherMatrix){
    Matrix3 result;

    for (int rowIt = 0; rowIt < 3; rowIt++) {
        for (int colIt = 0; colIt < 3; colIt++) {

        result.matrix[rowIt][colIt] = this->matrix[rowIt][colIt] - otherMatrix.matrix[rowIt][colIt];
        }
    }

    return result;
}

Matrix3 Matrix3::operator*(const Matrix3& otherMatrix){
    Matrix3 result;

    for (int rowIt = 0; rowIt < 3; rowIt++) {
        for (int colIt = 0; colIt < 3; colIt++) {
            double elementVal = 0.0;

            for (int tempIt = 0; tempIt < 3; tempIt++) {
                elementVal += matrix[rowIt][tempIt] * otherMatrix.matrix[tempIt][colIt];
            }

            result.matrix[rowIt][colIt] = elementVal;
        }
    }

    return result;
}

Matrix3& Matrix3::operator=(const Matrix3& otherMatrix){
    if (this == &otherMatrix) {
        return *this;
    }

    matrix = otherMatrix.matrix;
    return *this;
}

bool Matrix3::operator==(const Matrix3& otherMatrix) const{
    return this->matrix == otherMatrix.matrix;
}

std::ostream& operator<<(std::ostream& os, const Matrix3& matrix){

    for(const auto& it : matrix.matrix){
        os<<"[";
        for(double val : it){
            os<<" "<<val;
        }
        os<<"]"<<std::endl;
    }
    return os;
}
