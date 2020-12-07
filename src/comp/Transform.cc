#include <imgui/imgui.h>

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

const Mat4& Transform::GetMatrix()
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

void Transform::EditorHook()
{
  float uniformScale = mScale[0];
  ImGui::DragFloat3("Position", mTranslation.mD, 0.01f);
  ImGui::DragFloat("Scale", &uniformScale, 0.01f);
  SetUniformScale(uniformScale);
  mUpdated = false;
}

} // namespace Comp
