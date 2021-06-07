#include "Transform.h"

namespace Comp {

Transform::Transform():
  mScale({1.0f, 1.0f, 1.0f}),
  mRotation({1.0f, 0.0f, 0.0f, 0.0f}),
  mTranslation({0.0f, 0.0f, 0.0f}),
  mUpdated(false)
{}

const Vec3& Transform::GetScale() const
{
  return mScale;
}

const Quat& Transform::GetRotation() const
{
  return mRotation;
}

const Vec3& Transform::GetTranslation() const
{
  return mTranslation;
}

Quat Transform::GetWorldRotation(
  const World::Space& space, World::MemberId ownerId) const
{
  return GetParentWorldRotation(space, ownerId) * mRotation;
}

Vec3 Transform::GetWorldTranslation(
  const World::Space& space, World::MemberId ownerId)
{
  Mat4 worldMatrix = GetWorldMatrix(space, ownerId);
  return Math::ApplyToPoint(worldMatrix, {0.0f, 0.0f, 0.0f});
}

void Transform::SetUniformScale(float newUniformScale)
{
  mScale[0] = newUniformScale;
  mScale[1] = newUniformScale;
  mScale[2] = newUniformScale;
  mUpdated = false;
}

void Transform::SetScale(const Vec3& newScale)
{
  mScale = newScale;
  mUpdated = false;
}

void Transform::SetRotation(const Quat& newRotation)
{
  mRotation = newRotation;
  mRotation.Normalize();
  mUpdated = false;
}

void Transform::SetTranslation(const Vec3& newTranslation)
{
  mTranslation = newTranslation;
  mUpdated = false;
}

void Transform::SetWorldTranslation(
  const Vec3& worldTranslation,
  const World::Space& space,
  World::MemberId ownerId)
{
  SetTranslation(WorldToLocalTranslation(worldTranslation, space, ownerId));
}

Quat Transform::GetParentWorldRotation(
  const World::Space& space, World::MemberId ownerId) const
{
  const World::Member& member = space.GetConstMember(ownerId);
  Transform* pTransform = space.GetComponent<Transform>(member.Parent());
  if (pTransform == nullptr)
  {
    Math::Quaternion identity;
    identity.Identity();
    return identity;
  }
  return pTransform->GetWorldRotation(space, member.Parent());
}

Vec3 Transform::WorldToLocalTranslation(
  Vec3 worldTranslation, const World::Space& space, World::MemberId ownerId)
{
  const World::Member& member = space.GetConstMember(ownerId);
  Transform* pTransform = space.GetComponent<Transform>(member.Parent());
  if (pTransform == nullptr)
  {
    return worldTranslation;
  }
  Mat4 pInverseWorldMatrix =
    pTransform->GetInverseWorldMatrix(space, member.Parent());
  return Math::ApplyToPoint(pInverseWorldMatrix, worldTranslation);
}

const Mat4& Transform::GetLocalMatrix()
{
  if (!mUpdated)
  {
    // todo: Use matrix functions that automatically apply the desired
    // transformation to a matrix rather than creating a new matrix for every
    // transformation type.
    Mat4 scale, rotate, translate;
    Math::Scale(&scale, mScale);
    Math::Rotate(&rotate, mRotation);
    Math::Translate(&translate, mTranslation);
    mMatrix = translate * rotate * scale;
    mUpdated = true;
  }
  return mMatrix;
}

Mat4 Transform::GetInverseLocalMatrix()
{
  Mat4 inverseScale, inverseRotate, inverseTranslate;
  Math::Scale(&inverseScale, 1.0f / mScale);
  Math::Rotate(&inverseRotate, mRotation.Conjugate());
  Math::Translate(&inverseTranslate, -mTranslation);
  return inverseScale * inverseRotate * inverseTranslate;
}

Mat4 Transform::GetWorldMatrix(
  const World::Space& space, World::MemberId ownerId)
{
  const World::Member& member = space.GetConstMember(ownerId);
  World::MemberId parentId = member.Parent();
  Transform* parentTransform = space.GetComponent<Transform>(parentId);
  if (parentTransform == nullptr)
  {
    return GetLocalMatrix();
  }
  const Mat4& localTransformation = GetLocalMatrix();
  Mat4 parentTransformation = parentTransform->GetWorldMatrix(space, parentId);
  return parentTransformation * localTransformation;
}

Mat4 Transform::GetInverseWorldMatrix(
  const World::Space& space, World::MemberId ownerId)
{
  const World::Member& member = space.GetConstMember(ownerId);
  World::MemberId parentId = member.Parent();
  Transform* parentTransform = space.GetComponent<Transform>(parentId);
  if (parentTransform == nullptr)
  {
    return GetInverseLocalMatrix();
  }
  Mat4 inverseLocalMatrix = GetInverseLocalMatrix();
  Mat4 inverseParentMatrix =
    parentTransform->GetInverseWorldMatrix(space, parentId);
  return inverseLocalMatrix * inverseParentMatrix;
}

} // namespace Comp
