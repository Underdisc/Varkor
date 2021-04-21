#include "math/Geometry.h"

namespace Math {

// Ray /////////////////////////////////////////////////////////////////////////
void Ray::StartDirection(const Vec3& start, const Vec3& direction)
{
  mStart = start;
  Direction(direction);
}

void Ray::Direction(const Vec3& direction)
{
  mDirection = Normalize(direction);
}

const Vec3& Ray::Direction() const
{
  return mDirection;
}

Vec3 Ray::At(float t) const
{
  return mStart + mDirection * t;
}

float Ray::ClosestTTo(const Vec3& point) const
{
  return Math::Dot(mDirection, point - mStart);
}

Vec3 Ray::ClosestPointTo(const Vec3& point) const
{
  float t = Math::Dot(mDirection, point - mStart);
  return At(t);
}

float Ray::ClosestTTo(const Ray& other) const
{
  Vec3 sd = mStart - other.mStart;
  const Vec3& da = Direction();
  const Vec3& db = other.Direction();
  float dadb = Math::Dot(da, db);
  float dasd = Math::Dot(da, sd);
  float dbsd = Math::Dot(db, sd);
  float denom = 1.0f - dadb * dadb;
  LogAbortIf(denom == 0.0f, "The rays are parallel.");
  return (-dasd + dadb * dbsd) / denom;
}

Vec3 Ray::ClosestPointTo(const Ray& other) const
{
  return At(ClosestTTo(other));
}

// Plane ///////////////////////////////////////////////////////////////////////
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

// Functions ///////////////////////////////////////////////////////////////////
bool HasIntersection(const Ray& ray, const Plane& plane)
{
  if (Math::Dot(plane.Normal(), ray.Direction()) == 0.0f)
  {
    return false;
  }
  return true;
}

Vec3 Intersection(const Ray& ray, const Plane& plane)
{
  float np = Math::Dot(plane.Normal(), plane.mPoint);
  float ns = Math::Dot(plane.Normal(), ray.mStart);
  float nd = Math::Dot(plane.Normal(), ray.Direction());
  LogAbortIf(nd == 0.0f, "The Ray is perpendicular to the plane normal.");
  float t = (np - ns) / nd;
  return ray.At(t);
}

} // namespace Math
