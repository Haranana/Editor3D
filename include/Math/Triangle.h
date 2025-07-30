#ifndef TRIANGLE_H
#define TRIANGLE_H

template<typename T>
struct Triangle{
public:
    constexpr Triangle(const T& v1, const T& v2, const T& v3) : v1(v1), v2(v2), v3(v3) {};
    T v1,v2,v3;
};


#endif // TRIANGLE_H
