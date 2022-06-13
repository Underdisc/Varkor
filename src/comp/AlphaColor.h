#ifndef comp_AlphaColor_h
#define comp_AlphaColor_h

#include "math/Vector.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

struct AlphaColor
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VEdit(const World::Object& owner);

  static constexpr Vec4 smDefaultColor = {0.0f, 1.0f, 0.0f, 1.0f};
  Vec4 mColor;
};

} // namespace Comp

#endif
