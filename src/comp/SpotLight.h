#ifndef comp_SpotLight_h
#define comp_SpotLight_h

#include "gfx/HdrColor.h"
#include "math/Constants.h"
#include "math/Vector.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct SpotLight
{
  Vec3 mPosition;
  Vec3 mDirection;
  Gfx::HdrColor mAmbient;
  Gfx::HdrColor mDiffuse;
  Gfx::HdrColor mSpecular;
  float mConstant;
  float mLinear;
  float mQuadratic;
  float mInnerCutoff;
  float mOuterCutoff;

  static constexpr Vec3 smDefaultPosition = {0.0f, 0.0f, 0.0f};
  static constexpr Vec3 smDefaultDirection = {0.0f, -1.0f, 0.0f};
  static constexpr Gfx::HdrColor smDefaultAmbient = {1.0f, 1.0f, 1.0f, 0.1f};
  static constexpr Gfx::HdrColor smDefaultDiffuse = {1.0f, 1.0f, 1.0f, 0.5f};
  static constexpr Gfx::HdrColor smDefaultSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
  static constexpr float smDefaultConstant = 1.0f;
  static constexpr float smDefaultLinear = 0.35f;
  static constexpr float smDefaultQuadratic = 0.44f;
  static constexpr float smDefaultInnerCutoff = Math::nPi / 6.0f;
  static constexpr float smDefaultOuterCutoff = Math::nPi / 5.0f;

  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VEdit(const World::Object& owner);

  void SetInnerCutoff(float angle);
  void SetOuterCutoff(float angle);
  float GetInnerCutoff() const;
  float GetOuterCutoff() const;
};
#pragma pack(pop)

} // namespace Comp

#endif
