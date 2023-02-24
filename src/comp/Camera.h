#ifndef comp_Camera_h
#define comp_Camera_h

#include "math/Geometry.h"
#include "math/Matrix4.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Camera
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& cameraVal);
  void VDeserialize(const Vlk::Explorer& cameraEx);
  void VEdit(const World::Object& owner);

  void LocalLookAt(
    const Vec3& localPosition, const Vec3& localUp, const World::Object& owner);
  void WorldLookAt(
    const Vec3& worldPosition, const Vec3& worldUp, const World::Object& owner);

  Mat4 Proj() const;
  Mat4 View(const World::Object& owner) const;
  Mat4 InverseView(const World::Object& owner) const;

  Vec3 WorldTranslation(const World::Object& owner) const;
  Quat WorldRotation(const World::Object& owner) const;
  Vec3 WorldForward(const World::Object& owner) const;
  Vec3 WorldRight(const World::Object& owner) const;
  Vec3 WorldUp(const World::Object& owner) const;

  float ProjectedDistance(
    const World::Object& owner, const Vec3& worldTranslation) const;
  Vec3 StandardTranslationToWorldTranslation(
    Vec2 standardPosition, const World::Object& owner) const;
  Math::Ray StandardTranslationToWorldRay(
    const Vec2& standardPosition, const World::Object& owner) const;

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

#endif
