#ifndef math_Ray_h
#define math_Ray_h

#include "math/Vector.h"

namespace Math {

struct Ray {
public:
  Vec3 mStart;

private:
  Vec3 mDirection;

public:
  static Ray Points(const Vec3& a, const Vec3& b);
  static Ray StartDirection(const Vec3& start, const Vec3& direction);
  static Ray StartNormalizeDirection(const Vec3& start, const Vec3& direction);

  const Vec3& Direction() const;
  void Direction(const Vec3& direction);
  void NormalizeDirection(const Vec3& direction);

  Vec3 At(float t) const;
  float ClosestTTo(const Vec3& point) const;
  Vec3 ClosestPointTo(const Vec3& point) const;
  bool HasClosestTo(const Ray& other) const;
  float ClosestTTo(const Ray& other) const;
  Vec3 ClosestPointTo(const Ray& other) const;
};

} // namespace Math

#endif
