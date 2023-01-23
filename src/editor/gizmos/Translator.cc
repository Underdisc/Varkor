#include "editor/gizmos/Translator.h"
#include "Input.h"
#include "comp/Mesh.h"
#include "comp/Transform.h"
#include "editor/Editor.h"
#include "editor/gizmos/Gizmos.h"
#include "gfx/Material.h"
#include "gfx/Renderer.h"
#include "math/Constants.h"
#include "rsl/Library.h"

namespace Editor {
namespace Gizmos {

Vec3 Translate(
  const Vec3& translation,
  const Quat& referenceFrame,
  bool snapping,
  float snapInterval)
{
  return GetInstance<Translator>()->Run(
    translation, referenceFrame, snapping, snapInterval);
}

Translator::Translator(): mOperation(Operation::None)
{
  const char* handleNames[] = {"X", "Y", "Z", "Xy", "Xz", "Yz", "Xyz"};
  Rsl::Asset& translatorAsset = Rsl::AddAsset(smTranslatorAssetName);

  // Create all of the handles and handle materials.
  mParent = nSpace.CreateMember();
  nSpace.AddComponent<Comp::Transform>(mParent);
  for (int i = 0; i < smHandleCount; ++i) {
    mHandles[i] = nSpace.CreateChildMember(mParent);
    mHandleMaterialIds[i].Init(smTranslatorAssetName, handleNames[i]);
    Gfx::Material& material =
      translatorAsset.InitRes<Gfx::Material>(handleNames[i], nColorShaderId);
    material.mUniforms.Add<Vec4>("uColor") = smHandleColors[i];
  }
  translatorAsset.Finalize();

  Comp::Transform& xT = nSpace.AddComponent<Comp::Transform>(mX);
  xT.SetTranslation({0.5f, 0.0f, 0.0f});
  auto& xMesh = nSpace.AddComponent<Comp::Mesh>(mX);
  xMesh.mMeshId = nArrowMeshId;
  xMesh.mMaterialId = mHandleMaterialIds[(int)Operation::X];

  Comp::Transform& yT = nSpace.AddComponent<Comp::Transform>(mY);
  yT.SetTranslation({0.0f, 0.5f, 0.0f});
  Math::Quaternion yRotation;
  yRotation.AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  yT.SetRotation(yRotation);
  auto& yMesh = nSpace.AddComponent<Comp::Mesh>(mY);
  yMesh.mMeshId = nArrowMeshId;
  yMesh.mMaterialId = mHandleMaterialIds[(int)Operation::Y];

  Comp::Transform& zT = nSpace.AddComponent<Comp::Transform>(mZ);
  zT.SetTranslation({0.0f, 0.0f, 0.5f});
  Math::Quaternion zRotation;
  zRotation.AngleAxis(-Math::nPi / 2.0f, {0.0f, 1.0f, 0.0f});
  zT.SetRotation(zRotation);
  auto& zMesh = nSpace.AddComponent<Comp::Mesh>(mZ);
  zMesh.mMeshId = nArrowMeshId;
  zMesh.mMaterialId = mHandleMaterialIds[(int)Operation::Z];

  Comp::Transform& xyT = nSpace.AddComponent<Comp::Transform>(mXy);
  xyT.SetTranslation({0.5f, 0.5f, 0.0f});
  xyT.SetScale({0.15f, 0.15f, 0.01f});
  auto& xyMesh = nSpace.AddComponent<Comp::Mesh>(mXy);
  xyMesh.mMeshId = nCubeMeshId;
  xyMesh.mMaterialId = mHandleMaterialIds[(int)Operation::Xy];

  Comp::Transform& xzT = nSpace.AddComponent<Comp::Transform>(mXz);
  xzT.SetTranslation({0.5f, 0.0f, 0.5f});
  xzT.SetScale({0.15f, 0.01f, 0.15f});
  auto& xzMesh = nSpace.AddComponent<Comp::Mesh>(mXz);
  xzMesh.mMeshId = nCubeMeshId;
  xzMesh.mMaterialId = mHandleMaterialIds[(int)Operation::Xz];

  Comp::Transform& yzT = nSpace.AddComponent<Comp::Transform>(mYz);
  yzT.SetTranslation({0.0f, 0.5f, 0.5f});
  yzT.SetScale({0.01f, 0.15f, 0.15f});
  auto& yzMesh = nSpace.AddComponent<Comp::Mesh>(mYz);
  yzMesh.mMeshId = nCubeMeshId;
  yzMesh.mMaterialId = mHandleMaterialIds[(int)Operation::Yz];

  Comp::Transform& xyzT = nSpace.AddComponent<Comp::Transform>(mXyz);
  xyzT.SetUniformScale(0.13f);
  auto& xyzMesh = nSpace.AddComponent<Comp::Mesh>(mXyz);
  xyzMesh.mMeshId = nCubeMeshId;
  xyzMesh.mMaterialId = mHandleMaterialIds[(int)Operation::Xyz];
}

Translator::~Translator()
{
  nSpace.DeleteMember(mParent);
  Rsl::RemAsset(smTranslatorAssetName);
}

void Translator::SetNextOperation(
  const Vec3& translation, const Quat& referenceFrame)
{
  // Find out the new operation type.
  World::MemberId hoveredHandleId =
    Gfx::Renderer::HoveredMemberId(nSpace, nCamera.View(), nCamera.Proj());
  // clang-format off
    if (hoveredHandleId == mX) { mOperation = Operation::X; }
    else if (hoveredHandleId == mY) { mOperation = Operation::Y; }
    else if (hoveredHandleId == mZ) { mOperation = Operation::Z; }
    else if (hoveredHandleId == mXy) { mOperation = Operation::Xy; }
    else if (hoveredHandleId == mXz) { mOperation = Operation::Xz; }
    else if (hoveredHandleId == mYz) { mOperation = Operation::Yz; }
    else if (hoveredHandleId == mXyz) { mOperation = Operation::Xyz; }
    else { mOperation = Operation::None; return; }
  // clang-format on

  /// Initialize the translation ray or plane depending on the operation type.
  mTranslationRay.mStart = translation;
  mTranslationPlane.mPoint = translation;
  Vec3 xRotated = referenceFrame.Rotate({1.0f, 0.0f, 0.0f});
  Vec3 yRotated = referenceFrame.Rotate({0.0f, 1.0f, 0.0f});
  Vec3 zRotated = referenceFrame.Rotate({0.0f, 0.0f, 1.0f});
  switch (mOperation) {
  case Operation::X: mTranslationRay.Direction(xRotated); break;
  case Operation::Y: mTranslationRay.Direction(yRotated); break;
  case Operation::Z: mTranslationRay.Direction(zRotated); break;
  case Operation::Xy: mTranslationPlane.Normal(zRotated); break;
  case Operation::Xz: mTranslationPlane.Normal(yRotated); break;
  case Operation::Yz: mTranslationPlane.Normal(xRotated); break;
  case Operation::Xyz: mTranslationPlane.Normal(nCamera.Forward()); break;
  default: break;
  }

  // Set the mouse offset so it can be used to find the change in translation
  // during the following frames.
  Math::Ray mouseRay =
    nCamera.StandardPositionToRay(Input::StandardMousePosition());
  if (mOperation < Operation::Xy) {
    if (!mTranslationRay.HasClosestTo(mouseRay)) {
      mOperation = Operation::None;
      return;
    }
    mMouseOffset = mTranslationRay.ClosestPointTo(mouseRay) - translation;
    return;
  }
  else if (mOperation <= Operation::Xyz) {
    if (!Math::HasIntersection(mouseRay, mTranslationPlane)) {
      mOperation = Operation::None;
      return;
    }
    Vec3 intersection = Math::Intersection(mouseRay, mTranslationPlane);
    mMouseOffset = intersection - translation;
  }
}

Vec3 Translator::Run(
  const Vec3& translation,
  const Quat& referenceFrame,
  bool snapping,
  float snapInterval)
{
  SetParentTransformation(mParent, translation, referenceFrame);

  // Handle any switching between operations.
  if (!Input::MouseDown(Input::Mouse::Left) && mOperation != Operation::None) {
    auto& material =
      Rsl::GetRes<Gfx::Material>(mHandleMaterialIds[(int)mOperation]);
    material.mUniforms.Get<Vec4>("uColor") = smHandleColors[(int)mOperation];
    mOperation = Operation::None;
    return translation;
  }
  if (Input::MousePressed(Input::Mouse::Left)) {
    SetNextOperation(translation, referenceFrame);
    if (mOperation == Operation::None) {
      return translation;
    }
    Editor::nSuppressObjectPicking |= true;
    auto& material =
      Rsl::GetRes<Gfx::Material>(mHandleMaterialIds[(int)mOperation]);
    material.mUniforms.Get<Vec4>("uColor") = smActiveColor;
    return translation;
  }

  // Find a new translation depending on the active operation.
  Math::Ray mouseRay =
    nCamera.StandardPositionToRay(Input::StandardMousePosition());
  if (mOperation < Operation::Xy) {
    // Perform translation on an axis.
    if (!mTranslationRay.HasClosestTo(mouseRay)) {
      return translation;
    }
    Vec3 mousePoint = mTranslationRay.ClosestPointTo(mouseRay);
    Vec3 newTranslation = mousePoint - mMouseOffset;
    if (!snapping) {
      return newTranslation;
    }
    Vec3 delta = newTranslation - translation;
    delta = Math::ScaleToInterval(delta, snapInterval);
    return translation + delta;
  }
  else if (mOperation < Operation::Xyz) {
    // Perform translation on a plane.
    if (!Math::HasIntersection(mouseRay, mTranslationPlane)) {
      return translation;
    }
    Vec3 mousePoint = Math::Intersection(mouseRay, mTranslationPlane);
    Vec3 newTranslation = mousePoint - mMouseOffset;
    if (!snapping) {
      return newTranslation;
    }

    // Rotating the delta out of the reference frame leaves us with two non-zero
    // values. Snapping is applied to these values before reapplying the
    // reference frame to the delta.
    Vec3 delta = newTranslation - translation;
    delta = referenceFrame.Conjugate().Rotate(delta);
    for (int i = 0; i < 3; ++i) {
      delta[i] = Math::RoundToNearest(delta[i], snapInterval);
    }
    delta = referenceFrame.Rotate(delta);
    return translation + delta;
  }
  else if (mOperation == Operation::Xyz) {
    // Perform translation on the camera defined plane.
    if (Math::HasIntersection(mouseRay, mTranslationPlane)) {
      Vec3 mousePoint = Math::Intersection(mouseRay, mTranslationPlane);
      Vec3 newTranslation = mousePoint - mMouseOffset;
      return newTranslation;
    }
  }
  return translation;
}

} // namespace Gizmos
} // namespace Editor
