#ifndef math_Quaternion_h
#define math_Quaternion_h

#include <iostream>

#include "math/Vector.h"

namespace Math {

struct Quaternion
{
  float mA, mB, mC, mD;

  void Identity();
  void AngleAxis(float angle, Vec3 axis);
  void FromTo(Vec3 from, Vec3 to);

  Quaternion Conjugate() const;
  float Magnitude() const;
  void Normalize();
  Vec3 Axis() const;
  Vec3 EulerAngles() const;

  Vec3 Rotate(const Vec3& point) const;

  Quaternion& operator*=(const Quaternion& other);
};

Quaternion operator*(const Quaternion& a, const Quaternion& b);
std::ostream& operator<<(std::ostream& os, const Quaternion& quat);

} // namespace Math

typedef Math::Quaternion Quat;

#endif
