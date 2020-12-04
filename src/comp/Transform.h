#ifndef comp_Transform_h
#define comp_Transform_h

#include "math/Matrix4.h"
#include "math/Vector.h"

namespace Comp {

#pragma pack(push, 1)
struct Transform
{
  Transform();
  const Vec3& GetScale() const;
  const Quat& GetRotation() const;
  const Vec3& GetTranslation() const;
  void SetUniformScale(float newUniformScale);
  void SetScale(const Vec3& newScale);
  void SetRotation(const Quat& newRotation);
  void SetTranslation(const Vec3& newTranslation);
  const Mat4& GetMatrix();

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
