#ifndef math_Utility_h
#define math_Utility_h

namespace Math {

bool Near(float a, float b);
float Round(float value);
float RoundToNearest(float value, float interval);
template<typename T>
T Clamp(T minimum, T maximum, T value);

} // namespace Math

#include "math/Utility.hh"

#endif
