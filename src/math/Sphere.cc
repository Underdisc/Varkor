#include "math/Sphere.h"

namespace Math {

Sphere::Sphere() {}

Sphere::Sphere(const Vec3& center, float radius):
  mCenter(center), mRadius(radius)
{
  Init(center, radius);
}

void Sphere::Init(const Vec3& center, float radius)
{
  mCenter = center;
  mRadius = radius;
}

} // namespace Math