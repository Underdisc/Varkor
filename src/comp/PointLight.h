#ifndef comp_PointLight_h
#define comp_PointLight_h

#include "gfx/HdrColor.h"
#include "math/Vector.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct PointLight {
  Gfx::HdrColor mAmbient;
  Gfx::HdrColor mDiffuse;
  Gfx::HdrColor mSpecular;
  float mConstant;
  float mLinear;
  float mQuadratic;

  static const Gfx::HdrColor smDefaultAmbient;
  static const Gfx::HdrColor smDefaultDiffuse;
  static const Gfx::HdrColor smDefaultSpecular;
  static constexpr float smDefaultConstant = 1.0f;
  static constexpr float smDefaultLinear = 0.35f;
  static constexpr float smDefaultQuadratic = 0.44f;

  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VRenderable(const World::Object& owner);
  void VEdit(const World::Object& owner);
};
#pragma pack(pop)

} // namespace Comp

#endif
