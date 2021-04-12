#ifndef math_Ray_h
#define math_Ray_h

#include "math/Vector.h"

namespace Math {

struct Ray
{
  Ray(const Vec3& start, const Vec3& direction);
  void Direction(const Vec3& newDirection);
  const Vec3& Direction() const;
  Vec3 At(float t) const;
  Vec3 ClosestPointTo(const Ray& other) const;
  Vec3 ClosestPointTo(const Vec3& point) const;

public:
  Vec3 mStart;

private:
  Vec3 mDirection;
};

} // namespace Math

#endif
