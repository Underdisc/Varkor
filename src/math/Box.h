#ifndef math_Box_h
#define math_Box_h

#include "math/Quaternion.h"
#include "math/Vector.h"

namespace Math {

struct Box
{
  Box();
  Box(const Vec3& center, const Vec3& scale, const Quat& rotation);
  void Init(const Vec3& center, const Vec3& scale, const Quat& rotation);
  Vec3 Support(const Vec3& direction) const;

  Vec3 mCenter;
  Vec3 mScale;
  Quat mRotation;
};

bool HasIntersection(const Box& a, const Box& b);

} // namespace Math

#endif