#include "Math/Transform3D.h"
#include <cmath>

Matrix4 Transform3D::getTransMatrix() {
    return transMatrix;
}

Vector3 Transform3D::getAngles(bool inRadians) {
    if (inRadians) {
        return rotationParameters;
    }
    return Vector3(
        rotationParameters.x * 180.0 / M_PI,
        rotationParameters.y * 180.0 / M_PI,
        rotationParameters.z * 180.0 / M_PI
        );
}

Vector3 Transform3D::getPosition() {
    return translationParameters;
}

Vector3 Transform3D::getScales() {
    return scalingParameters;
}

void Transform3D::setPosition(Vector3 newPosition) {
    translationParameters = newPosition;
    updateTranslationMatrix();
    updateTransMatrix();
}

void Transform3D::setPositionX(double newX) {
    translationParameters.x = newX;
    updateTranslationMatrix();
    updateTransMatrix();
}

void Transform3D::setPositionY(double newY) {
    translationParameters.y = newY;
    updateTranslationMatrix();
    updateTransMatrix();
}

void Transform3D::setPositionZ(double newZ) {
    translationParameters.z = newZ;
    updateTranslationMatrix();
    updateTransMatrix();
}

void Transform3D::move(Vector3 newPositionDelta) {
    translationParameters = translationParameters + newPositionDelta;
    updateTranslationMatrix();
    updateTransMatrix();
}

void Transform3D::moveX(double xDelta) {
    translationParameters.x += xDelta;
    updateTranslationMatrix();
    updateTransMatrix();
}

void Transform3D::moveY(double yDelta) {
    translationParameters.y += yDelta;
    updateTranslationMatrix();
    updateTransMatrix();
}

void Transform3D::moveZ(double zDelta) {
    translationParameters.z += zDelta;
    updateTranslationMatrix();
    updateTransMatrix();
}

void Transform3D::setAngles(Vector3 newAngles, bool inRadians) {
    if (!inRadians) {
        newAngles = Vector3(
            newAngles.x * M_PI / 180.0,
            newAngles.y * M_PI / 180.0,
            newAngles.z * M_PI / 180.0
            );
    }
    rotationParameters = newAngles;
    updateRotationMatrix();
    updateTransMatrix();
}

void Transform3D::setAngleX(double newAngle, bool inRadians) {
    if (!inRadians) {
        newAngle *= M_PI / 180.0;
    }
    rotationParameters.x = newAngle;
    updateRotationMatrix();
    updateTransMatrix();
}

void Transform3D::setAngleY(double newAngle, bool inRadians) {
    if (!inRadians) {
        newAngle *= M_PI / 180.0;
    }
    rotationParameters.y = newAngle;
    updateRotationMatrix();
    updateTransMatrix();
}

void Transform3D::setAngleZ(double newAngle, bool inRadians) {
    if (!inRadians) {
        newAngle *= M_PI / 180.0;
    }
    rotationParameters.z = newAngle;
    updateRotationMatrix();
    updateTransMatrix();
}

void Transform3D::setScales(Vector3 newScales) {
    scalingParameters = newScales;
    updateScalingMatrix();
    updateTransMatrix();
}

void Transform3D::setScaleX(double newScale) {
    scalingParameters.x = newScale;
    updateScalingMatrix();
    updateTransMatrix();
}

void Transform3D::setScaleY(double newScale) {
    scalingParameters.y = newScale;
    updateScalingMatrix();
    updateTransMatrix();
}

void Transform3D::setScaleZ(double newScale) {
    scalingParameters.z = newScale;
    updateScalingMatrix();
    updateTransMatrix();
}

void Transform3D::updateTransMatrix() {
    transMatrix = translationMatrix * rotationMatrix * scalingMatrix;
}

void Transform3D::updateTranslationMatrix(){
    translationMatrix = Matrices4::translation(translationParameters.x, translationParameters.y, translationParameters.z);
}

void Transform3D::updateRotationMatrix(){
    rotationMatrix = Matrices4::rotation(rotationParameters.x, rotationParameters.y, rotationParameters.z);
}

void Transform3D::updateScalingMatrix(){
    scalingMatrix = Matrices4::scaling(scalingParameters.x, scalingParameters.y, scalingParameters.z);
}
