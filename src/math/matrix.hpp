#ifndef Matrix_h
#define Matrix_h

#include <iostream>

#include "vector.hpp"

template<typename T, unsigned int N>
class Matrix
{
public:
    void Clear();
    void Identity();
    void Scale(const Vector<T, N - 1>& scale);
    void Translate(const Vector<T, N - 1>& translation);
    T* Data();
    T* operator[](int row);
    Matrix<T, N>& operator=(const Matrix<T, N>& other);
    Matrix<T, N> operator*(const Matrix<T,N>& other);
    Matrix<T, N>& operator*=(const Matrix<T, N>& other);

private:
    T CalculateProductElement(int element_row, int element_column, const Matrix<T, N>& other);
    T _value[N][N];
};

template<typename T, unsigned int N>
void Matrix<T, N>::Clear()
{
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < N; ++c)
        {
            _value[r][c] = (T)0;
        }
    }
}

template<typename T, unsigned int N>
void Matrix<T, N>::Identity()
{
    Clear();
    for (int d = 0; d < N; ++d)
    {
        _value[d][d] = (T)1;
    }
}

template<typename T, unsigned int N>
void Matrix<T, N>::Scale(const Vector<T, N - 1>& scale)
{
    Clear();
    for (int i = 0; i < N - 1; ++i)
    {
        _value[i][i] = scale[i];
    }
    _value[N - 1][N - 1] = (T)1;
}

template<typename T, unsigned int N>
void Matrix<T, N>::Translate(const Vector<T, N - 1>& translate)
{
    Identity();
    for (int i = 0; i < N - 1; ++i)
    {
        _value[i][N - 1] = translate[i];
    }
}

template<typename T, unsigned int N>
T* Matrix<T, N>::Data()
{
    return _value[0];
}

template<typename T, unsigned int N>
T* Matrix<T, N>::operator[](int row)
{
    return _value[row];
}

template<typename T, unsigned int N>
Matrix<T, N>& Matrix<T, N>::operator=(const Matrix<T, N>& other)
{
    for (int r = 0; r < N; ++r)
    {
        for(int c = 0; c < N; ++c)
        {
            _value[r][c] = other._value[c][r];
        }
    }
}

template<typename T, unsigned int N>
Matrix<T, N> Matrix<T, N>::operator*(const Matrix<T,N>& other)
{
    Matrix<T, N> result;
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < N; ++c)
        {
            result[r][c] = CalculateProductElement(r, c, other);
        }
    }
    return result;
}

template<typename T, unsigned int N>
Matrix<T, N>& Matrix<T, N>::operator*=(const Matrix<T, N>& other)
{
    Matrix<T, N> result = *this * other;
    *this = result;
    return *this;
}

template<typename T, unsigned int N>
T Matrix<T, N>::CalculateProductElement(
    int element_row,
    int element_column,
    const Matrix<T, N>& other)
{
    T result = (T)0;
    for (int i = 0; i < N; ++i)
    {
        result += _value[element_row][i] * other._value[i][element_column];
    }
    return result;
}

template<typename T, unsigned int N>
std::ostream& operator<<(std::ostream& os, Matrix<T, N> mat)
{
    // todo: Add some code that keeps each column of the matrix at a consistent
    // width.
    for (int r = 0; r < N; ++r)
    {
        os << "[";
        for (int c = 0; c < N; ++c)
        {
            os << mat[r][c];
            if (c < N - 1)
            {
                os << ", ";
            }
        }
        os << "]" << std::endl;
    }
    return os;
}

typedef Matrix<float, 4> Mat4;

#endif