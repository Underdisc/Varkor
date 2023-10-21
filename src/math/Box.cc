#include "math/Box.h"

namespace Math {

void Box::Init(const Vec3& center, const Vec3& scale, const Quat& rotation)
{
  mCenter = center;
  mScale = scale;
  mRotation = rotation;
}

} // namespace Math