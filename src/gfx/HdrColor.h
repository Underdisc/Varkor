#ifndef gfx_Color_h
#define gfx_Color_h

#include <cstdlib>

#include "math/Vector.h"

namespace Gfx {

struct HdrColor {
  Vec3 mColor;
  float mIntensity;

  HdrColor() {}
  HdrColor(const std::initializer_list<float>& initList);
  HdrColor& operator=(const std::initializer_list<float>& initList);

  Vec3 TrueColor();
};

} // namespace Gfx

#endif
