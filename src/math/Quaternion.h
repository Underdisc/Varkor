#ifndef math_Quaternion_h
#define math_Quaternion_h

#include "math/Vector.h"

namespace Math {

struct Quaternion {
  union {
    struct {
      float mA, mB, mC, mD;
    };
    Vec4 mVec;
  };
  Quaternion();
  Quaternion(float a, float b, float c, float d);
  Quaternion(const Vec3& from, const Vec3& to);
  Quaternion(float angle, const Vec3& axis);
  void Identity();
  void AngleAxis(float angle, Vec3 axis);
  void FromTo(const Vec3& from, const Vec3& to);
  void BasisVectors(const Vec3& right, const Vec3& up, const Vec3& forward);
  static Quaternion InitAngleAxis(float angle, const Vec3& axis);
  static Quaternion InitFromTo(const Vec3& from, const Vec3& to);

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
typedef Math::Quaternion Quaternion;

#endif
