#ifndef math_Sphere_h
#define math_Sphere_h

#include "math/Vector.h"

namespace Math {

struct Sphere
{
  Vec3 mCenter;
  float mRadius;

  Sphere();
  Sphere(const Vec3& center, float radius);
  void Init(const Vec3& center, float radius);
};

} // namespace Math

#endif