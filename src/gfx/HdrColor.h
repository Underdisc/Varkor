#ifndef gfx_Color_h
#define gfx_Color_h

#include "math/Vector.h"

namespace Gfx {

struct HdrColor {
  Vec3 mColor;
  float mIntensity;

  Vec3 TrueColor() {
    return mColor * mIntensity;
  }
};

} // namespace Gfx

#endif
