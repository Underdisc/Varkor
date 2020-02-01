#ifndef Vector_h
#define Vector_h

template<typename T, unsigned int N>
class Vector
{
public:
    T operator[](int index) const;
    T& operator[](int index);
    Vector<T, N> operator+(const Vector<T, N>& other) const;
    Vector<T, N> operator-(const Vector<T, N>& other) const;
    Vector<T, N> operator*(const Vector<T, N>& other) const;
    Vector<T, N> operator*(T scalar) const;
    Vector<T, N>& operator+=(const Vector<T, N>& other);
    Vector<T, N>& operator-=(const Vector<T, N>& other);
    Vector<T, N>& operator*=(const Vector<T, N>& other);
    Vector<T, N>& operator*=(T scalar);

private:
    T _value[N];
};

template<typename T, unsigned int N>
T Vector<T, N>::operator[](int index) const
{
    return _value[index];
}

template<typename T, unsigned int N>
T& Vector<T, N>::operator[](int index)
{
    return _value[index];
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator+(const Vector<T, N>& other) const
{
    Vector<T, N> result;
    for(int i = 0; i < N; ++i)
    {
        result[i] = _value[i] + other[i];
    }
    return result;
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator-(const Vector<T, N>& other) const
{
    Vector<T, N> result;
    for (int i = 0; i < N; ++i)
    {
        result[i] = _value[i] - other[i];
    }
    return result;
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator*(const Vector<T, N>& other) const
{
    Vector<T, N> result;
    for (int i = 0; i < N; ++i)
    {
        result[i] = _value[i] * other[i];
    }
    return result;
}

template<typename T, unsigned int N>
Vector<T, N> Vector<T, N>::operator*(T scalar) const
{
    Vector<T, N> result;
    for (int i = 0; i < N; ++i)
    {
        result[i] = _value[i] * scalar;
    }
    return result;
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator+=(const Vector<T, N>& other)
{
    for (int i = 0; i < N; ++i)
    {
        _value[i] += other[i];
    }
    return *this;
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator-=(const Vector<T, N>& other)
{
    for (int i = 0; i < N; ++i)
    {
        _value[i] -= other[i];
    }
    return *this;
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator*=(const Vector<T, N>& other)
{
    for (int i = 0; i < N; ++i)
    {
        _value[i] *= other[i];
    }
    return *this;
}

template<typename T, unsigned int N>
Vector<T, N>& Vector<T, N>::operator*=(T scalar)
{
    for (int i = 0; i < N; ++i)
    {
        _value[i] *= scalar;
    }
    return *this;
}

typedef Vector<float, 2> Vec2;
typedef Vector<float, 3> Vec3;

#endif