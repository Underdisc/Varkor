#include <math.h>

#include "Error.h"
#include "math/Utility.h"

namespace Math {

template<typename T, unsigned int N>
const T& Vector<T, N>::operator[](int index) const {
  return mD[index];
}

template<typename T, unsigned int N>
T& Vector<T, N>::operator[](int index) {
  return mD[index];
}

template<typename T, unsigned int N>
template<unsigned int M>
Vector<T, N>::operator Vector<T, M>() const {
  Vector<T, M> result;
  for (int i = 0; i < N && i < M; ++i) {
    result[i] = mD[i];
  }
  for (int i = M - 1; i >= N; --i) {
    result[i] = (T)0;
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> operator+(const Vector<T, N>& a, const Vector<T, N>& b) {
  Vector<T, N> result;
  for (int i = 0; i < N; ++i) {
    result[i] = a[i] + b[i];
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> operator-(const Vector<T, N>& a, const Vector<T, N>& b) {
  Vector<T, N> result;
  for (int i = 0; i < N; ++i) {
    result[i] = a[i] - b[i];
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> operator*(const Vector<T, N>& vector, T scalar) {
  Vector<T, N> result;
  for (int i = 0; i < N; ++i) {
    result[i] = vector[i] * scalar;
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> operator*(T scaler, const Vector<T, N>& vector) {
  return vector * scaler;
}

template<typename T, unsigned int N>
Vector<T, N> operator/(const Vector<T, N>& vector, T scalar) {
  Vector<T, N> result;
  for (int i = 0; i < N; ++i) {
    result[i] = vector[i] / scalar;
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> operator/(T scalar, const Vector<T, N>& vector) {
  Vector<T, N> result;
  for (int i = 0; i < N; ++i) {
    result[i] = scalar / vector[i];
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N>& operator+=(Vector<T, N>& a, const Vector<T, N>& b) {
  for (int i = 0; i < N; ++i) {
    a[i] += b[i];
  }
  return a;
}

template<typename T, unsigned int N>
Vector<T, N>& operator-=(Vector<T, N>& a, const Vector<T, N>& b) {
  for (int i = 0; i < N; ++i) {
    a[i] -= b[i];
  }
  return a;
}

template<typename T, unsigned int N>
Vector<T, N>& operator*=(Vector<T, N>& vector, T scalar) {
  for (int i = 0; i < N; ++i) {
    vector[i] *= scalar;
  }
  return vector;
}

template<typename T, unsigned int N>
Vector<T, N>& operator/=(Vector<T, N>& vector, T scaler) {
  for (int i = 0; i < N; ++i) {
    vector[i] /= scaler;
  }
  return vector;
}

template<typename T, unsigned int N>
Vector<T, N> operator-(const Vector<T, N>& vector) {
  Vector<T, N> negated;
  for (int i = 0; i < N; ++i) {
    negated[i] = -vector[i];
  }
  return negated;
}

template<typename T, unsigned int N>
bool operator==(const Vector<T, N>& a, const Vector<T, N>& b) {
  for (int i = 0; i < N; ++i) {
    if (a[i] != b[i]) {
      return false;
    }
  }
  return true;
}

template<typename T, unsigned int N>
bool Near(const Vector<T, N>& a, const Vector<T, N>& b) {
  for (int i = 0; i < N; ++i) {
    if (!Near(a[i], b[i])) {
      return false;
    }
  }
  return true;
}

template<typename T, unsigned int N>
std::ostream& operator<<(std::ostream& os, const Vector<T, N>& vec) {
  os << "[";
  for (int i = 0; i < N; ++i) {
    os << vec[i];
    if (i < N - 1) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

template<typename T, unsigned int N>
T MagnitudeSq(const Vector<T, N>& vector) {
  return Dot(vector, vector);
}

template<unsigned int N>
float Magnitude(const Vector<float, N>& vector) {
  return sqrtf(MagnitudeSq(vector));
}

template<typename T, unsigned int N>
Vector<T, N> Normalize(const Vector<T, N>& vector) {
  T magnitude = Magnitude(vector);
  LogAbortIf(
    magnitude == (T)0, "Vector with magnitude of zero can't be normalized.");
  return vector / magnitude;
}

template<typename T>
Vector<T, 3> Cross(const Vector<T, 3>& a, const Vector<T, 3>& b) {
  Vector<T, 3> cross;
  cross[0] = a[1] * b[2] - a[2] * b[1];
  cross[1] = a[2] * b[0] - a[0] * b[2];
  cross[2] = a[0] * b[1] - a[1] * b[0];
  return cross;
}

template<typename T, unsigned int N>
T Dot(const Vector<T, N>& a, const Vector<T, N>& b) {
  T dot = (T)0;
  for (int i = 0; i < N; ++i) {
    dot += a[i] * b[i];
  }
  return dot;
}

template<typename T, unsigned int N>
Vector<T, N> ComponentwiseProduct(
  const Vector<T, N>& a, const Vector<T, N>& b) {
  Vector<T, N> result;
  for (int i = 0; i < N; ++i) {
    result[i] = a[i] * b[i];
  }
  return result;
}

template<typename T>
Vector<T, 3> PerpendicularTo(const Vector<T, 3>& vector) {
  Vector<T, 3> result;
  if (vector[0] == 0.0f && vector[2] == 0.0f) {
    result = {1.0f, 0.0f, 0.0f};
  }
  else {
    result = {-vector[2], 0.0f, vector[0]};
  }
  return result;
}

template<typename T, unsigned int N>
Vector<T, N> ScaleToInterval(Vector<T, N> vector, float interval) {
  float length = Math::Magnitude(vector);
  if (Math::Near(length, 0.0f)) {
    Vector<T, N> zeroVector {};
    return zeroVector;
  }
  float newLength = Math::RoundToNearest(length, interval);
  vector = vector / length;
  return vector * newLength;
}

template<typename T, unsigned int N>
Vector<T, N> ScaleComponentsToInterval(Vector<T, N> vector, float interval) {
  Vector<T, N> newVector;
  for (int i = 0; i < N; ++i) {
    newVector[i] = RoundToNearest(vector[i], interval);
  }
  return newVector;
}

} // namespace Math
