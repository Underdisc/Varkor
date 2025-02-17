#include "math/Plane.h"

namespace Math {

Plane::Plane() {}

Plane::Plane(const Vec3& a, const Vec3& b, const Vec3& c)
{
  Init(a, b, c);
}

void Plane::Init(const Vec3& a, const Vec3& b, const Vec3& c)
{
  mPoint = a;
  mNormal = Normalize(Cross(b - a, c - a));
}

void Plane::InitNormalized(const Vec3& point, const Vec3& normalizedNormal)
{
  mPoint = point;
  mNormal = normalizedNormal;
}

void Plane::PointNormal(const Vec3& point, const Vec3& normal)
{
  mPoint = point;
  Normal(normal);
}

void Plane::Normal(const Vec3& normal)
{
  mNormal = Normalize(normal);
}

const Vec3& Plane::Normal() const
{
  return mNormal;
}

} // namespace Math