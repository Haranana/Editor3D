#ifndef MATRICES_H
#define MATRICES_H

#include "Matrix3.h"
#include "Matrix4.h"
#include "cmath"

//A collection of functions that return commonly used 3x3 Matrices
namespace Matrices3 {
 Matrix3 identity();
 Matrix3 translation(double x, double y);
 Matrix3 scaling(double x, double y);
 Matrix3 rotation(double angle);
 Matrix3 shearing(double x, double y);

}

//A collection of functions that return commonly used 4x4 Matrices
namespace Matrices4 {
 Matrix4 identity();
 Matrix4 translation(double x, double y, double z);
 Matrix4 scaling(double x, double y, double z);
 Matrix4 rotation(double angleX, double angleY, double angleZ);
 Matrix4 rotationX(double angle);
 Matrix4 rotationY(double angle);
 Matrix4 rotationZ(double angle);
 Matrix3 Matrix4To3(const Matrix4& m);
}

#endif // MATRICES_H
