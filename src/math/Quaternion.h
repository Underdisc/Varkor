#ifndef math_Quaternion_h
#define math_Quaternion_h

#include <iostream>

#include "math/Vector.h"

namespace Math {

struct Quaternion
{
  Quaternion();
  Quaternion(float angle, Vec3 axis);
  Quaternion(float a, float b, float c, float d);

  void AngleAxis(float angle, Vec3 axis);
  void Normalize();

  Quaternion Conjugate() const;
  float Magnitude() const;
  Vec3 Axis() const;
  Vec3 EulerAngles() const;

  Quaternion& operator*=(const Quaternion& other);

  float mA, mB, mC, mD;
};

Quaternion operator*(const Quaternion& a, const Quaternion& b);
std::ostream& operator<<(std::ostream& os, const Quaternion& quat);

} // namespace Math

typedef Math::Quaternion Quat;

#endif
