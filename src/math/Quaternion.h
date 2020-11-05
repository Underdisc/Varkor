#ifndef math_Quaternion_h
#define math_Quaternion_h

#include <iostream>

#include "math/Vector.h"

namespace Math {

struct Quaternion
{
  float mA, mB, mC, mD;

  void AngleAxis(float angle, Vec3 axis);
  Quaternion Conjugate() const;
  Quaternion& operator*=(const Quaternion& other);
};

Quaternion operator*(const Quaternion& a, const Quaternion& b);
std::ostream& operator<<(std::ostream& os, const Quaternion& quat);

} // namespace Math

typedef Math::Quaternion Quat;

#endif
