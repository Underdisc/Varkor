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
  void LocalLookAt(
    const Vec3& localPosition, const Vec3& localUp, const World::Object& owner);
  void WorldLookAt(
    const Vec3& worldPosition, const Vec3& worldUp, const World::Object& owner);
  Mat4 Proj() const;
  Vec3 StandardToWorldPosition(
    Vec2 standardPosition, const Mat4& inverseView) const;

  enum class ProjectionType
  {
    Perspective,
    Orthographic
  };

  ProjectionType mProjectionType;
  union
  {
    float mFov;
    float mHeight;
  };
  float mNear;
  float mFar;

  static const char* smProjectionTypeNames[];
  static float smDefaultFov;
  static float smDefaultHeight;
  static float smMinimumPerspectiveNear;
  static float smMinimumOrthographicNear;
  static float smMaximumFar;
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
