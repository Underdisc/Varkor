#include "math/Plane.h"
#include "math/Constants.h"

namespace Math {

Plane Plane::Points(const Vec3& a, const Vec3& b, const Vec3& c) {
  Plane plane;
  plane.mPoint = b;
  plane.mNormal = Normalize(Cross(a - b, c - b));
  return plane;
}

Plane Plane::Newell(const Ds::Vector<Vec3>& points) {
  Plane plane;
  plane.mNormal = Vec3::Zero();
  plane.mPoint = Vec3::Zero();
  for (int p = 0; p < points.Size(); ++p) {
    const Vec3& a = points[p];
    const Vec3& b = points[(p + 1) % points.Size()];
    plane.mNormal[0] += (a[1] - b[1]) * (a[2] + b[2]);
    plane.mNormal[1] += (a[2] - b[2]) * (a[0] + b[0]);
    plane.mNormal[2] += (a[0] - b[0]) * (a[1] + b[1]);
    plane.mPoint += points[p];
  }
  plane.mNormal = Normalize(plane.mNormal);
  plane.mPoint /= (float)points.Size();
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
  return HalfSpaceContains(point, nEpsilon);
}

bool Plane::HalfSpaceContains(const Vec3& point, float epsilon) const {
  return Distance(point) <= epsilon;
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

bool Near(const Plane& a, const Plane& b) {
  Vec4 aHalfspace = (Vec4)a.Normal();
  aHalfspace[3] = -Dot(a.Normal(), a.mPoint);
  Vec4 bHalfspace = (Vec4)b.Normal();
  bHalfspace[3] = -Dot(b.Normal(), b.mPoint);
  return Near(aHalfspace, bHalfspace);
}

} // namespace Math
