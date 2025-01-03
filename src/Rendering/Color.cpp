#include "Rendering/Color.h"

Color::Color(int B, int G, int R , int A){
    set(B,G,R,A);
}

Color::Color(const Color& otherColor){
    set(otherColor.B,otherColor.G,otherColor.R,otherColor.A);
}

Color::Color() {}

void Color::set(int B, int G, int R, int A){
    this->A = A;
    this->B = B;
    this->G = G;
    this->R = R;
}

Color& Color::operator=(const Color& otherColor){
    if(this != &otherColor){
        this->B = otherColor.B;
        this->G = otherColor.G;
        this->R = otherColor.R;
        this->A = otherColor.A;
    }
    return *this;
}

bool Color::operator==(const Color& otherColor) const{
    return this->B==otherColor.B && this->G==otherColor.G && this->R==otherColor.R;
}

bool Color::operator!=(const Color& otherColor) const{
    return this->B!=otherColor.B || this->G!=otherColor.G || this->R!=otherColor.R;;
}
