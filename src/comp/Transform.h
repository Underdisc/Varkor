#ifndef comp_Transform_h
#define comp_Transform_h

namespace World {
struct Object;
}

#include "math/Matrix4.h"
#include "math/Vector.h"
#include "world/Space.h"
#include "world/Types.h"

namespace Comp {

#pragma pack(push, 1)
struct Transform
{
  void VInit();

  const Vec3& GetScale() const;
  const Quat& GetRotation() const;
  const Vec3& GetTranslation() const;
  Quat GetWorldRotation(
    const World::Space& space, World::MemberId ownerId) const;
  Vec3 GetWorldTranslation(const World::Space& space, World::MemberId ownerId);

  void SetUniformScale(float newUniformScale);
  void SetScale(const Vec3& newScale);
  void SetRotation(const Quat& newRotation);
  void SetTranslation(const Vec3& newTranslation);
  void SetWorldTranslation(
    const Vec3& worldTranslation,
    const World::Space& space,
    World::MemberId ownerId);

  Quat GetParentWorldRotation(
    const World::Space& space, World::MemberId ownerId) const;
  Vec3 WorldToLocalTranslation(
    Vec3 worldTranslation, const World::Space& space, World::MemberId ownerId);

  const Mat4& GetLocalMatrix();
  Mat4 GetInverseLocalMatrix();
  Mat4 GetWorldMatrix(const World::Space& space, World::MemberId ownerId);
  Mat4 GetInverseWorldMatrix(
    const World::Space& space, World::MemberId ownerId);

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
