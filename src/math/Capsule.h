#ifndef math_Capsule_h
#define math_Capsule_h

#include "math/Vector.h"

namespace Math {

struct Capsule {
  Vec3 mCenters[2];
  float mRadius;
};

} // namespace Math

#endif
