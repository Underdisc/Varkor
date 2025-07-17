#ifndef comp_DirectionalLight_h
#define comp_DirectionalLight_h

#include "gfx/HdrColor.h"
#include "math/Vector.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct DirectionalLight {
  Gfx::HdrColor mAmbient;
  Gfx::HdrColor mDiffuse;
  Gfx::HdrColor mSpecular;

  static const Gfx::HdrColor smDefaultAmbient;
  static const Gfx::HdrColor smDefaultDiffuse;
  static const Gfx::HdrColor smDefaultSpecular;

  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VRenderable(const World::Object& owner);
  void VEdit(const World::Object& owner);
};
#pragma pack(pop)

} // namespace Comp

#endif
