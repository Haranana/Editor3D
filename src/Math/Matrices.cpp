#include "Math/Matrices.h"

using std::cos;
using std::sin;

namespace Matrices3 {
Matrix3 identity() {
    return Matrix3({{
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    }});
}

Matrix3 translation(double x, double y) {
    return Matrix3({{
        {1, 0, x},
        {0, 1, y},
        {0, 0, 1}
    }});
}

Matrix3 scaling(double x, double y) {
    return Matrix3({{
        {x, 0, 0},
        {0, y, 0},
        {0, 0, 1}
    }});
}

Matrix3 rotation(double angle) {
    return Matrix3({{
        {cos(angle), -sin(angle), 0},
        {sin(angle), cos(angle), 0},
        {0, 0, 1}
    }});
}

Matrix3 shearing(double x, double y) {
    return Matrix3({{
        {1, x, 0},
        {y, 1, 0},
        {0, 0, 1}
    }});
}
}

namespace Matrices4 {
Matrix4 identity() {
    return Matrix4({{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }});
}

Matrix4 translation(double x, double y, double z) {
    return Matrix4({{
        {1, 0, 0, x},
        {0, 1, 0, y},
        {0, 0, 1, z},
        {0, 0, 0, 1}
    }});
}

Matrix4 scaling(double x, double y, double z) {
    return Matrix4({{
        {x, 0, 0, 0},
        {0, y, 0, 0},
        {0, 0, z, 0},
        {0, 0, 0, 1}
    }});
}

Matrix4 rotation(double angleX , double angleY, double angleZ) {
    return Matrices4::rotationZ(angleZ) * Matrices4::rotationY(angleY) * Matrices4::rotationX(angleX);
}

Matrix4 rotationX(double angle) {
    return Matrix4({{
        {1,       0,        0,       0},
        {0,  cos(angle),  -sin(angle),       0},
        {0,  sin(angle),   cos(angle),       0},
        {0,       0,        0,       1}
    }});
}

Matrix4 rotationY(double angle) {
    return Matrix4({{
        { cos(angle),  0, sin(angle), 0},
        {      0,  1,      0, 0},
        {-sin(angle), 0, cos(angle), 0},
        {      0,  0,      0, 1}
    }});
}

Matrix4 rotationZ(double angle) {
    return Matrix4({{
        {cos(angle), -sin(angle), 0, 0},
        {sin(angle),  cos(angle), 0, 0},
        {     0,       0, 1, 0},
        {     0,       0, 0, 1}
    }});
}

Matrix3 Matrix4To3(const Matrix4& m){
    return Matrix3({{
        {{m.get(0,0), m.get(0,1), m.get(0,2)}},
        {{m.get(1,0), m.get(1,1), m.get(1,2)}},
        {{m.get(2,0), m.get(2,1), m.get(2,2)}}
    }});
}


}

namespace LightMatrices{
Matrix4 lightView(const Vector3& eye , const Vector3& target, const Vector3& up){
    Vector3 f = (target - eye).normalize();
    Vector3 r = f.crossProduct(up).normalize();
    Vector3 u = r.crossProduct(f);

    return Matrix4({{
        {  r.x,   r.y,   r.z,  -r.dotProduct(eye) },
        {  u.x,   u.y,   u.z,  -u.dotProduct(eye) },
        { -f.x,  -f.y,  -f.z,   f.dotProduct(eye) },
        {    0,     0,     0,               1      }
    }});
}

Matrix4 orthogonalLightProjection(double left, double right, double bottom, double top, double near, double far){
    return Matrix4({{
        { 2/(right-left), 0,        0,          -(right+left)/(right-left) },
        { 0,       2/(top-bottom),  0,          -(top+bottom)/(top-bottom) },
        { 0,       0,       -2/(far-near),    -(far+near)/(far-near) },
        { 0,       0,        0,           1           }
    }});
}
Matrix4 PerspectiveLightProjection(double fovY, double near, double far, double screenAscpect){
    double s = 1.0 / std::tan(fovY * 0.5);
    return Matrix4({{
        { s/screenAscpect, 0,  0,               0 },
        { 0,        s,  0,               0 },
        { 0,        0, -(far+near)/(far-near),    -2*far*near/(far-near) },
        { 0,        0, -1,               0 }
    }});
}
}
