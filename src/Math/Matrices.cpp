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

Matrix4 rotation(double angleX , double angleY, double angleZ) {
    return Matrices4::rotationZ(angleZ) * Matrices4::rotationY(angleY) * Matrices4::rotationX(angleX);
}
}
