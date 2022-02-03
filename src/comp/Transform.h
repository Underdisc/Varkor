#ifndef comp_Transform_h
#define comp_Transform_h

namespace Vlk {
struct Value;
struct Explorer;
} // namespace Vlk

namespace World {
struct Object;
}

#include "math/Matrix4.h"
#include "math/Vector.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Transform
{
  void VInit();
  void VSerialize(Vlk::Value& transformVal);
  void VDeserialize(const Vlk::Explorer& transformEx);

  const Vec3& GetScale() const;
  const Quat& GetRotation() const;
  const Vec3& GetTranslation() const;
  Quat GetWorldRotation(const World::Object& object) const;
  Vec3 GetWorldTranslation(const World::Object& object);

  void SetUniformScale(float newUniformScale);
  void SetScale(const Vec3& newScale);
  void SetRotation(const Quat& newRotation);
  void SetTranslation(const Vec3& newTranslation);
  void SetWorldTranslation(
    const Vec3& worldTranslation, const World::Object& object);

  Quat GetParentWorldRotation(const World::Object& object) const;
  Vec3 WorldToLocalTranslation(
    const Vec3& worldTranslation, const World::Object& object);

  const Mat4& GetLocalMatrix();
  Mat4 GetInverseLocalMatrix();
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
