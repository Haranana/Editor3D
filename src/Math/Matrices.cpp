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
    return Matrix4(
        {{
        {cos(angleY)*cos(angleZ), sin(angleX)*sin(angleY)*cos(angleZ)-cos(angleX)*sin(angleZ), cos(angleX)*sin(angleY)*cos(angleZ)+sin(angleX)*sin(angleZ), 0},
        {cos(angleY)*cos(angleZ), sin(angleX)*sin(angleY)*sin(angleZ)-cos(angleX)*cos(angleZ), cos(angleX)*sin(angleY)*sin(angleZ)+sin(angleX)*cos(angleZ), 0},
        {-sin(angleY), sin(angleX)*cos(angleY), cos(angleX)*cos(angleY), 0},
        {0, 0, 0, 1}
        }});
}
}
