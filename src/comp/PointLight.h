#ifndef comp_PointLight_h
#define comp_PointLight_h

#include "math/Vector.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct PointLight
{
  Vec3 mPosition;
  Vec3 mAmbient;
  Vec3 mDiffuse;
  Vec3 mSpecular;
  float mConstant;
  float mLinear;
  float mQuadratic;

  static constexpr Vec3 smDefaultPosition = {0.0f, 0.0f, 0.0f};
  static constexpr Vec3 smDefaultAmbient = {0.1f, 0.1f, 0.1f};
  static constexpr Vec3 smDefaultDiffuse = {0.5f, 0.5f, 0.5f};
  static constexpr Vec3 smDefaultSpecular = {1.0f, 1.0f, 1.0f};
  static constexpr float smDefaultConstant = 1.0f;
  static constexpr float smDefaultLinear = 0.35f;
  static constexpr float smDefaultQuadratic = 0.44f;

  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VEdit(const World::Object& owner);
};
#pragma pack(pop)

} // namespace Comp

#endif
