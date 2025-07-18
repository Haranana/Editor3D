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

Color Color::operator+(const Color& color)const{
    return Color(
        std::min(255 , std::max(0 ,this->B + color.B)),
        std::min(255 ,  std::max(0 ,this->G + color.G)),
        std::min(255 ,  std::max(0, this->R + color.R))
        );
}
Color Color::operator-(const Color& color)const{ return Color(
        std::min(255 , std::max(0 ,this->B - color.B)),
        std::min(255 ,  std::max(0 ,this->G - color.G)),
        std::min(255 ,  std::max(0, this->R - color.R))
        );}

Color Color::operator*(const Color& color)const{return Color(
        std::min(255 , std::max(0 ,this->B * color.B)),
        std::min(255 ,  std::max(0 ,this->G * color.G)),
        std::min(255 ,  std::max(0, this->R * color.R))
        );}
Color Color::operator/(const Color& color)const{return Color(
        std::min(255 , std::max(0 ,this->B / color.B)),
        std::min(255 ,  std::max(0 ,this->G / color.G)),
        std::min(255 ,  std::max(0, this->R / color.R))
        );}

Color Color::operator*(int val)const{return Color(
        std::min(255 , std::max(0 ,this->B * val)),
        std::min(255 ,  std::max(0 ,this->G * val)),
        std::min(255 ,  std::max(0, this->R * val))
        );}
Color Color::operator+(int val)const{return Color(
        std::min(255 , std::max(0 ,this->B + val)),
        std::min(255 ,  std::max(0 ,this->G + val)),
        std::min(255 ,  std::max(0, this->R + val))
        );}
Color Color::operator-(int val)const{return Color(
        std::min(255 , std::max(0 ,this->B - val)),
        std::min(255 ,  std::max(0 ,this->G - val)),
        std::min(255 ,  std::max(0, this->R - val))
        );}
Color Color::operator/(int val)const{return Color(
        std::min(255 , std::max(0 ,this->B / val)),
        std::min(255 ,  std::max(0 ,this->G / val)),
        std::min(255 ,  std::max(0, this->R / val))
        );}

std::ostream& operator<<(std::ostream& os, const Color& color){

    os<<"[R: "<<color.R<<" , G: "<<color.G<<" , B: "<<color.B<<" , A: "<<color.A<<"]";

    return os;
}

