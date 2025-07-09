#ifndef COLOR_H
#define COLOR_H

#include <iostream>

/*
 * Color class
 * elements of Color, both in bits of QImage in used format, and in this class
 * are always in order: Blue, Green, Red, Alpha
 * alpha should not be changed unless there's veeeery good reason to do so
*/
class Color{
public:
    Color(int B, int G, int R, int A = 255);
    Color(const Color& otherColor);
    Color();

    void set(int B, int G, int R, int A = 255);

    bool operator==(const Color& otherColor) const;
    bool operator!=(const Color& otherColor) const;
    Color& operator=(const Color& otherColor);

    friend std::ostream& operator<<(std::ostream& os, const Color& color);

    int B = 0;
    int G = 0;
    int R = 0;
    int A = 255;
};

#endif // COLOR_H
