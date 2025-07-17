#include "gfx/HdrColor.h"

namespace Gfx {

HdrColor::HdrColor(const std::initializer_list<float>& initList) {
  *this = initList;
}

HdrColor& HdrColor::operator=(const std::initializer_list<float>& initList) {
  Assert(initList.size() == 4);
  int listPos = 0;
  auto it = initList.begin();
  for (int i = 0; i < 3; ++i) {
    mColor[i] = *(it++);
  }
  mIntensity = *it;
  return *this;
}

Vec3 HdrColor::TrueColor() {
  return mColor * mIntensity;
}

} // namespace Gfx
