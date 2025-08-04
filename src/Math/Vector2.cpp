#include "Math/Vector2.h"


Vector2& Vector2::operator=(const Vector2& otherVector){
    if(this != &otherVector){
        this->x = otherVector.x;
        this->y = otherVector.y;
    }
    return *this;
}

bool Vector2::operator==(const Vector2& otherVector) const{
    return this->x==otherVector.x && this->y==otherVector.y;
}

Vector2 Vector2::operator+(const Vector2& otherVector) const{
    return Vector2(this->x + otherVector.x , this->y + otherVector.y);
}

Vector2 Vector2::operator-(const Vector2& otherVector)const{
    return Vector2(this->x - otherVector.x , this->y - otherVector.y);
}

Vector2 Vector2::operator*(const Vector2& otherVector)const{
    return Vector2(this->x * otherVector.x , this->y * otherVector.y);
}

Vector2 Vector2::operator/(const Vector2& otherVector)const{
    return Vector2(this->x / otherVector.x , this->y / otherVector.y);
}



Vector2 Vector2::operator+(double val)const{
    return Vector2(this->x + val , this->y + val);
}

Vector2 Vector2::operator-(double val)const{
    return Vector2(this->x - val , this->y - val);
}

Vector2 Vector2::operator*(double val)const{
    return Vector2(this->x * val , this->y * val);
}

Vector2 Vector2::operator/(double val)const{
    return Vector2(this->x / val , this->y / val);
}


std::ostream& operator<<(std::ostream& os, const Vector2& Vector2) {
    os << "(" << Vector2.x << "," << Vector2.y << ")";
    return os;
}
