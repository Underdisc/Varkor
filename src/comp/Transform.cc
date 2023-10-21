#include <imgui/imgui.h>

#include "Input.h"
#include "comp/Transform.h"
#include "editor/Utility.h"
#include "editor/gizmos/Gizmos.h"
#include "editor/gizmos/Rotator.h"
#include "editor/gizmos/Scalor.h"
#include "editor/gizmos/Translator.h"
#include "math/Constants.h"

namespace Comp {

void Transform::VInit(const World::Object& owner)
{
  mScale = {1.0f, 1.0f, 1.0f};
  mRotation = {1.0f, 0.0f, 0.0f, 0.0f};
  mTranslation = {0.0f, 0.0f, 0.0f};
  mUpdated = false;
}

void Transform::VSerialize(Vlk::Value& transformVal)
{
  Vlk::Value& scaleVal = transformVal("Scale")[{3}];
  Vlk::Value& rotationVal = transformVal("Rotation")[{4}];
  Vlk::Value& translationVal = transformVal("Translation")[{3}];
  for (int i = 0; i < 3; ++i) {
    scaleVal[i] = mScale[i];
    rotationVal[i] = mRotation[i];
    translationVal[i] = mTranslation[i];
  }
  rotationVal[3] = mRotation[3];
}

void Transform::VDeserialize(const Vlk::Explorer& transformEx)
{
  Vlk::Explorer scaleEx = transformEx("Scale");
  Vlk::Explorer rotationEx = transformEx("Rotation");
  Vlk::Explorer translationEx = transformEx("Translation");
  mRotation[0] = rotationEx[0].As<float>(1.0f);
  for (int i = 0; i < 3; ++i) {
    mScale[i] = scaleEx[i].As<float>(1.0f);
    mRotation[i + 1] = rotationEx[i + 1].As<float>(0.0f);
    mTranslation[i] = translationEx[i].As<float>(0.0f);
  }
  mUpdated = false;
}

void Transform::VEdit(const World::Object& owner)
{
  // Display all of the transformation component's parameters.
  Vec3 translation = GetTranslation();
  ImGui::PushItemWidth(-60.0f);
  bool translationDragged =
    ImGui::DragFloat3("Translation", translation.mD, 0.01f);
  if (translationDragged) {
    SetTranslation(translation);
  }

  Vec3 scale = GetScale();
  bool scaleDragged = ImGui::DragFloat3("Scale", scale.mD, 0.01f);
  if (scaleDragged) {
    SetScale(scale);
  }

  bool rotationDragged = Editor::RotationEdit(&mRotation);
  if (rotationDragged) {
    mUpdated = false;
  }
  ImGui::PopItemWidth();
  Editor::Gizmos::ImGuiOptions();
}

void Transform::VGizmoEdit(const World::Object& owner)
{
  // Get a rotation depending on the current reference frame.
  using namespace Editor::Gizmos;
  Quat referenceFrameRotation;
  switch (nReferenceFrame) {
  case ReferenceFrame::World:
    referenceFrameRotation = {1.0f, 0.0f, 0.0f, 0.0f};
    break;
  case ReferenceFrame::Parent:
    referenceFrameRotation = GetParentWorldRotation(owner);
    break;
  case ReferenceFrame::Relative:
    referenceFrameRotation = GetWorldRotation(owner);
    break;
  }

  // Display the gizmo for the current mode we are in.
  Vec3 worldTranslation = GetWorldTranslation(owner);
  if (nMode == Mode::Translate) {
    Vec3 newTranslation = Editor::Gizmo<Translator>::Next().Run(
      worldTranslation, referenceFrameRotation);
    if (!Math::Near(newTranslation, worldTranslation)) {
      SetWorldTranslation(newTranslation, owner);
    }
  }
  else if (nMode == Mode::Scale) {
    referenceFrameRotation = GetWorldRotation(owner);
    Vec3 scale = GetScale();
    Vec3 newScale = Editor::Gizmo<Scalor>::Next().Run(
      scale, worldTranslation, referenceFrameRotation);
    if (!Math::Near(newScale, scale)) {
      SetScale(newScale);
    }
  }
  else {
    Quat worldRotation = GetWorldRotation(owner);
    Quat newWorldRotation = Editor::Gizmo<Rotator>::Next().Run(
      worldRotation, worldTranslation, referenceFrameRotation);
    if (!Math::Near(worldRotation.mVec, newWorldRotation.mVec)) {
      SetWorldRotation(newWorldRotation, owner);
    }
  }
}

const Vec3& Transform::GetTranslation() const
{
  return mTranslation;
}

Vec3 Transform::GetWorldTranslation(const World::Object& object)
{
  Mat4 worldMatrix = GetWorldMatrix(object);
  return Math::ApplyToPoint(worldMatrix, {0.0f, 0.0f, 0.0f});
}

void Transform::SetTranslation(const Vec3& newTranslation)
{
  mTranslation = newTranslation;
  mUpdated = false;
}

void Transform::SetWorldTranslation(
  const Vec3& worldTranslation, const World::Object& object)
{
  SetTranslation(WorldToLocalTranslation(worldTranslation, object));
}

Vec3 Transform::WorldToLocalTranslation(
  const Vec3& worldTranslation, const World::Object& object)
{
  World::Object pObject = object.Parent();
  Transform* pTransform = pObject.TryGetComponent<Transform>();
  if (pTransform == nullptr) {
    return worldTranslation;
  }
  Mat4 pInverseWorldMatrix = pTransform->GetInverseWorldMatrix(pObject);
  return Math::ApplyToPoint(pInverseWorldMatrix, worldTranslation);
}

const Vec3& Transform::GetScale() const
{
  return mScale;
}

void Transform::SetScale(const Vec3& newScale)
{
  mScale = newScale;
  mUpdated = false;
}

void Transform::SetUniformScale(float newUniformScale)
{
  mScale[0] = newUniformScale;
  mScale[1] = newUniformScale;
  mScale[2] = newUniformScale;
  mUpdated = false;
}

const Quat& Transform::GetRotation() const
{
  return mRotation;
}

Quat Transform::GetWorldRotation(const World::Object& object) const
{
  Quat worldRotation = GetParentWorldRotation(object) * mRotation;
  worldRotation.Normalize();
  return worldRotation;
}

Quat Transform::GetParentWorldRotation(const World::Object& object) const
{
  World::Object pObject = object.Parent();
  Transform* pTransform = pObject.TryGetComponent<Transform>();
  if (pTransform == nullptr) {
    return Math::Quaternion(1.0f, 0.0, 0.0f, 0.0f);
  }
  return pTransform->GetWorldRotation(pObject);
}

void Transform::SetRotation(const Quat& newRotation)
{
  mRotation = newRotation;
  mRotation.Normalize();
  mUpdated = false;
}

void Transform::SetWorldRotation(
  const Quat& newWorldRotation, const World::Object& object)
{
  World::Object pObject = object.Parent();
  Transform* pTransform = pObject.TryGetComponent<Transform>();
  if (pTransform == nullptr) {
    mRotation = newWorldRotation;
  }
  else {
    Quat parentRotation = pTransform->GetWorldRotation(pObject);
    mRotation = parentRotation.Conjugate() * newWorldRotation;
  }
  mRotation.Normalize();
  mUpdated = false;
}

const Mat4& Transform::GetLocalMatrix()
{
  if (!mUpdated) {
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

Mat4 Transform::GetInverseLocalMatrix() const
{
  Mat4 inverseScale, inverseRotate, inverseTranslate;
  Math::Scale(&inverseScale, 1.0f / mScale);
  Math::Rotate(&inverseRotate, mRotation.Conjugate());
  Math::Translate(&inverseTranslate, -mTranslation);
  return inverseScale * inverseRotate * inverseTranslate;
}

Mat4 Transform::GetWorldMatrix(const World::Object& object)
{
  World::Object pObject = object.Parent();
  Transform* pTransform = pObject.TryGetComponent<Transform>();
  if (pTransform == nullptr) {
    return GetLocalMatrix();
  }
  const Mat4& localTransformation = GetLocalMatrix();
  Mat4 parentTransformation = pTransform->GetWorldMatrix(pObject);
  return parentTransformation * localTransformation;
}

Mat4 Transform::GetInverseWorldMatrix(const World::Object& object)
{
  World::Object pObject = object.Parent();
  Transform* pTransform = pObject.TryGetComponent<Transform>();
  if (pTransform == nullptr) {
    return GetInverseLocalMatrix();
  }
  Mat4 inverseLocalMatrix = GetInverseLocalMatrix();
  Mat4 inverseParentMatrix = pTransform->GetInverseWorldMatrix(pObject);
  return inverseLocalMatrix * inverseParentMatrix;
}

} // namespace Comp
