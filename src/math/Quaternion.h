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
  Quaternion(float angle, const Vec3& axis);
  Quaternion(const Vec3& from, const Vec3& to);
  Quaternion(const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis);
  void InitIdentity();
  void InitAngleAxis(float angle, const Vec3& axis);
  void InitFromTo(const Vec3& from, const Vec3& to);
  void InitBasisVectors(
    const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis);
  static Quaternion Identity();
  static Quaternion AngleAxis(float angle, Vec3 axis);
  static Quaternion FromTo(const Vec3& from, const Vec3& to);
  static Quaternion BasisVectors(
    const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis);

  Quaternion Interpolate(float t) const;
  Quaternion Conjugate() const;
  float Magnitude() const;
  void Normalize();
  Vec3 Axis() const;
  Vec3 EulerAngles() const;
  Vec3 XBasisAxis() const;
  Vec3 YBasisAxis() const;
  Vec3 ZBasisAxis() const;
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
