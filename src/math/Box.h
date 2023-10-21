#ifndef math_Box_h
#define math_Box_h

#include "math/Quaternion.h"
#include "math/Vector.h"

namespace Math {

struct Box
{
  void Init(const Vec3& center, const Vec3& scale, const Quat& rotation);

  Vec3 mCenter;
  Vec3 mScale;
  Quat mRotation;
};

} // namespace Math

#endif