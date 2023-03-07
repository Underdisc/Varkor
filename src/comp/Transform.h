#ifndef comp_Transform_h
#define comp_Transform_h

#include "math/Matrix4.h"
#include "math/Vector.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Transform
{
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& transformVal);
  void VDeserialize(const Vlk::Explorer& transformEx);
  void VEdit(const World::Object& owner);
  void VGizmoEdit(const World::Object& owner);

  const Vec3& GetTranslation() const;
  Vec3 GetWorldTranslation(const World::Object& object);
  void SetTranslation(const Vec3& newTranslation);
  void SetWorldTranslation(
    const Vec3& worldTranslation, const World::Object& object);
  Vec3 WorldToLocalTranslation(
    const Vec3& worldTranslation, const World::Object& object);

  const Vec3& GetScale() const;
  void SetScale(const Vec3& newScale);
  void SetUniformScale(float newUniformScale);

  const Quat& GetRotation() const;
  Quat GetWorldRotation(const World::Object& object) const;
  Quat GetParentWorldRotation(const World::Object& object) const;
  void SetRotation(const Quat& newRotation);
  void SetWorldRotation(const Quat& worldRotation, const World::Object& object);

  const Mat4& GetLocalMatrix();
  Mat4 GetInverseLocalMatrix() const;
  Mat4 GetWorldMatrix(const World::Object& object);
  Mat4 GetInverseWorldMatrix(const World::Object& object);

private:
  Vec3 mScale;
  Quat mRotation;
  Vec3 mTranslation;

  bool mUpdated;
  Mat4 mMatrix;
};
#pragma pack(pop)

} // namespace Comp

#endif
