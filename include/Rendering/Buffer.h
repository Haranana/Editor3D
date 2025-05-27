#ifndef BUFFER_H
#define BUFFER_H
#include <vector>
template<typename T>
class Buffer{
public:
    Buffer(std::size_t rows, std::size_t cols, T defaultValue);


    T* operator[](std::size_t it);
    const T* operator[](std::size_t it) const;

    std::size_t getRows() const;
    std::size_t getCols() const;
    std::size_t size() const;
    void clear();

private:
    std::size_t rows, cols;
    std::vector<T> data;
    T defaultValue;
};


template<typename T>
Buffer<T>::Buffer(std::size_t rows, std::size_t cols, T defaultValue)
    : rows(rows)
    , cols(cols)
    , defaultValue(defaultValue)
    , data(rows * cols , defaultValue)
{}

template<typename T>
T* Buffer<T>::operator[](std::size_t it) {
    return data.data() + it*cols;
}

template<typename T>
const T* Buffer<T>::operator[](std::size_t it) const {
    return data.data() + it*cols;
}

template<typename T>
std::size_t Buffer<T>::getRows() const {
    return rows;
}

template<typename T>
std::size_t Buffer<T>::getCols() const {
    return cols;
}

template<typename T>
std::size_t Buffer<T>::size() const {
    return rows * cols;
}

template<typename T>
void Buffer<T>::clear(){
    std::fill(data.begin(), data.end(), defaultValue);
}

#endif // BUFFER_H
