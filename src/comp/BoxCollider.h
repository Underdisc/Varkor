#ifndef comp_BoxCollider_h
#define comp_BoxCollider_h

#include "math/Box.h"
#include "world/Object.h"

namespace Comp {

struct BoxCollider
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& modelVal);
  void VDeserialize(const Vlk::Explorer& modelEx);
  void VRenderable(const World::Object& owner);
  void VEdit(const World::Object& owner);
  void VGizmoEdit(const World::Object& owner);

  Math::Box mBox;
  bool mShow;
};

} // namespace Comp

#endif