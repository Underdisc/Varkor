#ifndef comp_Camera_h
#define comp_Camera_h

#include "editor/HookInterface.h"
#include "math/Matrix4.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Camera
{
  void VInit();
  void VSerialize(Vlk::Value& cameraVal);
  void VDeserialize(const Vlk::Explorer& cameraEx);
  Mat4 Proj() const;
  Vec3 StandardToWorldPosition(
    Vec2 standardPosition, const Mat4& inverseView) const;

  float mFov;
  float mNear;
  float mFar;

  static float smDefaultFov;
  static float smDefaultNear;
  static float smDefaultFar;
};
#pragma pack(pop)

} // namespace Comp

namespace Editor {

template<>
struct Hook<Comp::Camera>: public HookInterface
{
  bool Edit(const World::Object& object);
};

} // namespace Editor

#endif
