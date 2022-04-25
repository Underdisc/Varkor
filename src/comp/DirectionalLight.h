#ifndef comp_DirectionalLight_h
#define comp_DirectionalLight_h

#include "editor/HookInterface.h"
#include "math/Vector.h"
#include "vlk/Valkor.h"

namespace Comp {

#pragma pack(push, 1)
struct DirectionalLight
{
  Vec3 mDirection;
  Vec3 mAmbient;
  Vec3 mDiffuse;
  Vec3 mSpecular;

  static constexpr Vec3 smDefaultDirection = {0.0f, -1.0f, 0.0f};
  static constexpr Vec3 smDefaultAmbient = {0.1f, 0.1f, 0.1f};
  static constexpr Vec3 smDefaultDiffuse = {0.5f, 0.5f, 0.5f};
  static constexpr Vec3 smDefaultSpecular = {1.0f, 1.0f, 1.0f};

  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
};
#pragma pack(pop)

} // namespace Comp

namespace Editor {

template<>
struct Hook<Comp::DirectionalLight>: public HookInterface
{
  bool Edit(const World::Object& object);
};

} // namespace Editor

#endif
