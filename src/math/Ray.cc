#include "math/Ray.h"
#include "Error.h"

namespace Math {

Ray::Ray(const Vec3& start, const Vec3& direction): mStart(start)
{
  Direction(direction);
}

void Ray::Direction(const Vec3& newDirection)
{
  mDirection = Normalize(newDirection);
}

const Vec3& Ray::Direction() const
{
  return mDirection;
}

Vec3 Ray::At(float t) const
{
  return mStart + mDirection * t;
}

Vec3 Ray::ClosestPointTo(const Ray& other) const
{
  Vec3 sd = mStart - other.mStart;
  const Vec3& da = Direction();
  const Vec3& db = other.Direction();
  float dadb = Math::Dot(da, db);
  float dasd = Math::Dot(da, sd);
  float dbsd = Math::Dot(db, sd);
  float denom = 1.0f - dadb * dadb;
  LogAbortIf(denom == 0.0f, "The rays are parallel.");
  float ta = (-dasd + dadb * dbsd) / denom;
  return At(ta);
}

} // namespace Math
