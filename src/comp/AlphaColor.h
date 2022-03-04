#ifndef comp_AlphaColor_h
#define comp_AlphaColor_h

#include "math/Vector.h"
#include "world/Object.h"

namespace Comp {

struct AlphaColor
{
  void VInit(const World::Object& owner);
  Vec4 mAlphaColor;
};

} // namespace Comp

#endif
