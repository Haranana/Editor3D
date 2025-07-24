#ifndef BUFFER_H
#define BUFFER_H
#include <vector>


/*
 * Data structure similiar in usage to list of lists but
 * faster thanks to actually being single list
 * useful when structure is cleared/filled very often, such as z-buffer and id-buffer
*/
template<typename T>
class Buffer{
public:
    Buffer(std::size_t rows, std::size_t cols, T defaultValue);


    T* operator[](std::size_t it);
    const T* operator[](std::size_t it) const;

    std::size_t getRows() const;
    std::size_t getCols() const;
    std::size_t size() const; //returns number of elements both default and non-default
    std::size_t nonEmptyElements() const; //returns nubmer of elements that are different than default v alue
    bool isEmpty() const;
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
std::size_t Buffer<T>::nonEmptyElements() const{
    std::size_t result = 0;
    for(const T& it : data){
        if(data!=defaultValue) result++;
    }
    return result;
}

template<typename T>
bool Buffer<T>::isEmpty() const{
    for(const T& it : data){
        if(data!=defaultValue) return false;
    }
    return true;
}

template<typename T>
void Buffer<T>::clear(){
    std::fill(data.begin(), data.end(), defaultValue);
}

#endif // BUFFER_H
