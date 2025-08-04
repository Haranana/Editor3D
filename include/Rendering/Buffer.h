#ifndef BUFFER_H
#define BUFFER_H
#include <vector>
#include <iostream>

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
    bool exists(size_t row, size_t col) const; //checks whether given row and col are not out of bounds
    void logNonEmptyElements() const;
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
        if(it!=defaultValue) result++;
    }
    return result;
}

template<typename T>
bool Buffer<T>::isEmpty() const{
    for(const T& it : data){
        if(it!=defaultValue) return false;
    }
    return true;
}

template<typename T>
bool Buffer<T>::exists(size_t row, size_t col) const{
    return row >= 0 && row < getRows() && col >= 0 && col < getCols();
}

template<typename T>
void Buffer<T>::clear(){
    std::fill(data.begin(), data.end(), defaultValue);
}

template<typename T>
void Buffer<T>::logNonEmptyElements() const{
    for(std::size_t row=0; row<rows; row++){
        for(std::size_t col=0; col<cols; col++){
            if(data[row*cols+col]!=defaultValue) std::cout<<"row|col|val:  "<<row<<":"<<col<<":"<<data[row*cols+col]<<std::endl;
        }
    }
}

#endif // BUFFER_H
