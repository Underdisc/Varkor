#ifndef math_Utility_h
#define math_Utility_h

namespace Math {

bool Near(float a, float b);
bool Near(float a, float b, float epsilon);
float Sign(float value);
float Abs(float value);
float Round(float value);
float RoundToNearest(float value, float interval);
template<typename T>
const T& Max(const T& a, const T& b);
template<typename T>
const T& Min(const T& a, const T& b);
template<typename T>
T Clamp(T minimum, T maximum, T value);
template<typename T>
T Lerp(const T& start, const T& end, float t);

} // namespace Math

#include "math/Utility.hh"

#endif
