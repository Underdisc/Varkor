#ifndef math_Quaternion_h
#define math_Quaternion_h

#include "math/Vector.h"

namespace Math {

struct Quaternion
{
  union
  {
    struct
    {
      float mA, mB, mC, mD;
    };
    Vec4 mVec;
  };
  Quaternion();
  Quaternion(float a, float b, float c, float d);
  void Identity();
  void AngleAxis(float angle, Vec3 axis);
  void FromTo(Vec3 from, Vec3 to);

  Quaternion Interpolate(float t) const;
  Quaternion Conjugate() const;
  float Magnitude() const;
  void Normalize();
  Vec3 Axis() const;
  Vec3 EulerAngles() const;

  Vec3 Rotate(const Vec3& point) const;

  Quaternion& operator*=(const Quaternion& other);
  const float& operator[](int index) const;
  float& operator[](int index);
};

Quaternion operator*(const Quaternion& a, const Quaternion& b);

} // namespace Math

typedef Math::Quaternion Quat;

#endif
