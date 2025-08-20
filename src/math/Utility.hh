namespace Math {

template<typename T>
const T& Max(const T& a, const T& b) {
  if (a > b) {
    return a;
  }
  return b;
}

template<typename T>
const T& Min(const T& a, const T& b) {
  if (a < b) {
    return a;
  }
  return b;
}

template<typename T>
T Clamp(T minimum, T maximum, T value) {
  if (value < minimum) {
    return minimum;
  }
  if (value > maximum) {
    return maximum;
  }
  return value;
}

template<typename T>
T Lerp(const T& start, const T& end, float t) {
  return start * (1.0f - t) + end * t;
}

} // namespace Math
