#include "math/Geometry.h"
#include "math/Utility.h"

namespace Math {

// Ray /////////////////////////////////////////////////////////////////////////
void Ray::InitNormalized(const Vec3& start, const Vec3& normalizedDirection)
{
  mStart = start;
  mDirection = normalizedDirection;
}

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

bool Ray::HasClosestTo(const Ray& other) const
{
  float directionDot = Math::Dot(mDirection, other.mDirection);
  return !Math::Near(directionDot, 1.0f) && !Math::Near(directionDot, -1.0f);
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

// Sphere //////////////////////////////////////////////////////////////////////
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

// Functions ///////////////////////////////////////////////////////////////////
bool HasIntersection(const Ray& ray, const Plane& plane)
{
  if (Math::Dot(plane.Normal(), ray.Direction()) == 0.0f) {
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

RaySphere Intersection(const Ray& ray, const Sphere& sphere)
{
  Vec3 relativeStart = ray.mStart - sphere.mCenter;
  float a = Dot(ray.Direction(), ray.Direction());
  float b = 2 * Dot(relativeStart, ray.Direction());
  float c = Dot(relativeStart, relativeStart) - sphere.mRadius * sphere.mRadius;
  float disc = b * b - 4 * a * c;

  RaySphere intersection;
  if (disc < 0) {
    intersection.mCount = 0;
    return intersection;
  }
  if (Near(disc, 0)) {
    intersection.mCount = 1;
    float t = -b / (2 * a);
    intersection.mPoints[0] = ray.At(t);
    return intersection;
  }
  intersection.mCount = 2;
  float t = sqrt(disc);
  intersection.mPoints[0] = ray.At((-b - t) / (2 * a));
  intersection.mPoints[1] = ray.At((-b + t) / (2 * a));
  return intersection;
}

} // namespace Math
