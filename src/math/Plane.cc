#include "math/Plane.h"
#include "math/Constants.h"

namespace Math {

Plane Plane::Points(const Vec3& a, const Vec3& b, const Vec3& c) {
  Plane plane;
  plane.mPoint = b;
  plane.mNormal = Normalize(Cross(a - b, c - b));
  return plane;
}

Plane Plane::PointNormal(const Vec3& point, const Vec3& normal) {
  Plane plane;
  plane.mPoint = point;
  plane.mNormal = normal;
  return plane;
}

Plane Plane::PointNormalizeNormal(const Vec3& point, const Vec3& normal) {
  Plane plane;
  plane.mPoint = point;
  plane.mNormal = Normalize(normal);
  return plane;
}

bool Plane::HalfSpaceContains(const Vec3& point) const {
  return Distance(point) <= nEpsilon;
}

float Plane::Distance(const Vec3& point) const {
  Vec3 pp = point - mPoint;
  return Math::Dot(pp, mNormal);
}

Vec3 Plane::ClosestTo(const Vec3& point) const {
  return point - Distance(point) * mNormal;
}

void Plane::Normal(const Vec3& normal) {
  mNormal = Normalize(normal);
}

const Vec3& Plane::Normal() const {
  return mNormal;
}

} // namespace Math
