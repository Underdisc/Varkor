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

void Transform::SetWorldTranslation(
  const Vec3& worldTranslation,
  const World::Space& space,
  World::MemberId ownerId)
{
  const World::Member& member = space.GetConstMember(ownerId);
  Transform* pTransform = space.GetComponent<Transform>(member.Parent());
  if (pTransform == nullptr)
  {
    SetTranslation(worldTranslation);
    return;
  }
  Mat4 parentWorldMatrix = pTransform->GetWorldMatrix(space, member.Parent());
  Mat4 inverseParentMatrix = Math::Inverse(parentWorldMatrix);
  SetTranslation(Math::ApplyToPoint(inverseParentMatrix, worldTranslation));
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

Mat4 Transform::GetWorldMatrix(
  const World::Space& space, World::MemberId ownerId)
{
  const World::Member& member = space.GetConstMember(ownerId);
  if (!member.HasParent())
  {
    return GetLocalMatrix();
  }
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

} // namespace Comp
