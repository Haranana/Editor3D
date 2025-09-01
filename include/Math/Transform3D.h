#ifndef TRANSFORM3D_H
#define TRANSFORM3D_H

#include "Matrix4.h"
#include "Vector3.h"
#include "Matrices.h"

class Transform3D{
public:

    Matrix4 getTransMatrix() const;

    Vector3 getAngles(bool inRadians = true) const;
    Vector3 getPosition() const;
    Vector3 getScales() const;

    void setPosition(Vector3 newPosition);
    void setPositionX(double newX);
    void setPositionY(double newY);
    void setPositionZ(double newZ);
    void move(Vector3 newPositionDelta);
    void moveX(double xDelta);
    void moveY(double yDelta);
    void moveZ(double zDelta);

    void setAngles(Vector3 newAngles , bool inRadians = true);
    void setAngleX(double newAngle , bool inRadians = true);
    void setAngleY(double newAngle , bool inRadians = true);
    void setAngleZ(double newAngle , bool inRadians = true);

    void setScales(Vector3 newScales);
    void setScaleX(double newScale);
    void setScaleY(double newScale);
    void setScaleZ(double newScale);

private:

    void updateTransMatrix();
    void updateTranslationMatrix();
    void updateRotationMatrix();
    void updateScalingMatrix();

    Vector3 translationParameters = Vector3(0,0,0);
    Vector3 rotationParameters = Vector3(0,0,0);
    Vector3 scalingParameters = Vector3(1,1,1);

    Matrix4 transMatrix = Matrices4::identity();
    Matrix4 translationMatrix = Matrices4::translation(0,0,0);
    Matrix4 rotationMatrix = Matrices4::rotation(0,0,0);
    Matrix4 scalingMatrix = Matrices4::scaling(1,1,1);

};

#endif // TRANSFORM3D_H
