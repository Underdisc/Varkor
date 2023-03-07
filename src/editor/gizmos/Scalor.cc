#include "editor/gizmos/Scalor.h"
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

Vec3 Scale(
  const Vec3& scale,
  const Vec3& translation,
  const Quat& referenceFrame,
  bool snapping,
  float snapInterval)
{
  return GetInstance<Scalor>()->Run(
    scale, translation, referenceFrame, snapping, snapInterval);
}

Scalor::Scalor(): mOperation(Operation::None)
{
  // Create all of the handles.
  const char* handleNames[] = {"X", "Y", "Z", "Xy", "Xz", "Yz", "Xyz"};
  Rsl::Asset& scalorAsset = Rsl::AddAsset(smScalorAssetName);
  mParent = nSpace.CreateMember();
  nSpace.AddComponent<Comp::Transform>(mParent);
  for (int i = 0; i < smHandleCount; ++i) {
    mHandles[i] = nSpace.CreateChildMember(mParent);
    mHandleMaterialIds[i].Init(smScalorAssetName, handleNames[i]);
    Gfx::Material& material =
      scalorAsset.InitRes<Gfx::Material>(handleNames[i], nColorShaderId);
    material.mUniforms.Add<Vec4>("uColor") = smHandleColors[i];
  }
  scalorAsset.Finalize();

  Comp::Transform& xT = nSpace.AddComponent<Comp::Transform>(mX);
  xT.SetTranslation({0.5f, 0.0f, 0.0f});
  auto& xMesh = nSpace.AddComponent<Comp::Mesh>(mX);
  xMesh.mMeshId = nScaleMeshId;
  xMesh.mMaterialId = mHandleMaterialIds[(int)Operation::X];

  Comp::Transform& yT = nSpace.AddComponent<Comp::Transform>(mY);
  yT.SetTranslation({0.0f, 0.5f, 0.0f});
  Math::Quaternion yRotation;
  yRotation.AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  yT.SetRotation(yRotation);
  auto& yMesh = nSpace.AddComponent<Comp::Mesh>(mY);
  yMesh.mMeshId = nScaleMeshId;
  yMesh.mMaterialId = mHandleMaterialIds[(int)Operation::Y];

  Comp::Transform& zT = nSpace.AddComponent<Comp::Transform>(mZ);
  zT.SetTranslation({0.0f, 0.0f, 0.5f});
  Math::Quaternion zRotation;
  zRotation.AngleAxis(-Math::nPi / 2.0f, {0.0f, 1.0f, 0.0f});
  zT.SetRotation(zRotation);
  auto& zMesh = nSpace.AddComponent<Comp::Mesh>(mZ);
  zMesh.mMeshId = nScaleMeshId;
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
  yzMesh.mMaterialId = mHandleMaterialIds[(int)Operation::X];

  Comp::Transform& xyzT = nSpace.AddComponent<Comp::Transform>(mXyz);
  xyzT.SetUniformScale(1.2f);
  auto& xyzMesh = nSpace.AddComponent<Comp::Mesh>(mXyz);
  xyzMesh.mMeshId = nTorusMeshId;
  xyzMesh.mMaterialId = mHandleMaterialIds[(int)Operation::Xyz];
}

Scalor::~Scalor()
{
  nSpace.DeleteMember(mParent);
  Rsl::RemAsset(smScalorAssetName);
}

void Scalor::SetNextOperation(
  const Vec3& scale, const Vec3& translation, const Quat& referenceFrame)
{
  // Find the next operation type.
  const World::Object cameraObject = nCamera.GetObject();
  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  World::MemberId hoveredHandleId =
    Gfx::Renderer::HoveredMemberId(nSpace, cameraObject);
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

  // Set values to be used for the selected operation during following frames.
  mScaleRay.mStart = translation;
  mScalePlane.mPoint = translation;
  Vec3 xRotated = referenceFrame.Rotate({1.0f, 0.0f, 0.0f});
  Vec3 yRotated = referenceFrame.Rotate({0.0f, 1.0f, 0.0f});
  Vec3 zRotated = referenceFrame.Rotate({0.0f, 0.0f, 1.0f});
  Vec3 forward = cameraComp.WorldForward(cameraObject);
  switch (mOperation) {
  case Operation::X: mScaleRay.Direction(xRotated); break;
  case Operation::Y: mScaleRay.Direction(yRotated); break;
  case Operation::Z: mScaleRay.Direction(zRotated); break;
  case Operation::Xy: mScalePlane.Normal(zRotated); break;
  case Operation::Xz: mScalePlane.Normal(yRotated); break;
  case Operation::Yz: mScalePlane.Normal(xRotated); break;
  case Operation::Xyz: mScalePlane.Normal(forward); break;
  default: break;
  }
  Math::Ray mouseRay = cameraComp.StandardTranslationToWorldRay(
    Input::StandardMousePosition(), cameraObject);
  if (mOperation < Operation::Xy) {
    if (!mScaleRay.HasClosestTo(mouseRay)) {
      mOperation = Operation::None;
      return;
    }
    mMousePosition = mScaleRay.ClosestPointTo(mouseRay);
  }
  else if (mOperation < Operation::Xyz) {
    if (!Math::HasIntersection(mouseRay, mScalePlane)) {
      mOperation = Operation::None;
      return;
    }
    mMousePosition = Math::Intersection(mouseRay, mScalePlane);
  }
  else {
    mMousePosition = Math::Intersection(mouseRay, mScalePlane);
    mUniformScaleDirection = Math::Normalize(mMousePosition - translation);
    mNormalizedStartScale = Math::Normalize(scale);
  }
}

Vec3 Scalor::Run(
  const Vec3& scale,
  const Vec3& translation,
  const Quat& referenceFrame,
  bool snapping,
  float snapInterval)
{
  // Set the transformations for the handles that change every frame.
  SetParentTransformation(mParent, translation, referenceFrame);
  const World::Object cameraObject = nCamera.GetObject();
  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  Vec3 torusDirection = cameraComp.WorldTranslation(cameraObject) - translation;
  Math::Quaternion xyzRotation;
  xyzRotation.FromTo({1.0f, 0.0f, 0.0f}, torusDirection);
  xyzRotation = referenceFrame.Conjugate() * xyzRotation;
  Comp::Transform& xyzTransform = nSpace.GetComponent<Comp::Transform>(mXyz);
  xyzTransform.SetRotation(xyzRotation);

  // Handle any switching between operations.
  if (!Input::MouseDown(Input::Mouse::Left) && mOperation != Operation::None) {
    auto& material =
      Rsl::GetRes<Gfx::Material>(mHandleMaterialIds[(int)mOperation]);
    material.mUniforms.Get<Vec4>("uColor") = smHandleColors[(int)mOperation];
    mOperation = Operation::None;
    return scale;
  }
  if (Input::MousePressed(Input::Mouse::Left)) {
    SetNextOperation(scale, translation, referenceFrame);
    if (mOperation == Operation::None) {
      return scale;
    }
    Editor::nSuppressObjectPicking |= true;
    auto& material =
      Rsl::GetRes<Gfx::Material>(mHandleMaterialIds[(int)mOperation]);
    material.mUniforms.Get<Vec4>("uColor") = smActiveColor;
    return scale;
  }

  // Perform the desired operation.
  Vec3 axes;
  switch (mOperation) {
  case Operation::X: axes = {1.0, 0.0f, 0.0f}; break;
  case Operation::Y: axes = {0.0f, 1.0f, 0.0f}; break;
  case Operation::Z: axes = {0.0f, 0.0f, 1.0f}; break;
  case Operation::Xy: axes = {1.0f, 1.0f, 0.0f}; break;
  case Operation::Xz: axes = {1.0f, 0.0f, 1.0f}; break;
  case Operation::Yz: axes = {0.0f, 1.0f, 1.0f}; break;
  case Operation::Xyz: axes = {1.0f, 1.0f, 1.0f}; break;
  default: break;
  }
  Math::Ray mouseRay = cameraComp.StandardTranslationToWorldRay(
    Input::StandardMousePosition(), cameraObject);
  if (mOperation < Operation::Xy) {
    if (!mScaleRay.HasClosestTo(mouseRay)) {
      return scale;
    }
    Vec3 newMousePosition = mScaleRay.ClosestPointTo(mouseRay);
    Vec3 mouseDelta = newMousePosition - mMousePosition;
    float deltaMagnitude = Math::Dot(mouseDelta, mScaleRay.Direction());
    if (!snapping) {
      mMousePosition = newMousePosition;
      return scale + axes * deltaMagnitude;
    }
    deltaMagnitude = Math::RoundToNearest(deltaMagnitude, snapInterval);
    mMousePosition += mScaleRay.Direction() * deltaMagnitude;
    return scale + axes * deltaMagnitude;
  }
  else if (mOperation < Operation::Xyz) {
    if (!Math::HasIntersection(mouseRay, mScalePlane)) {
      return scale;
    }
    Vec3 newMousePosition = Math::Intersection(mouseRay, mScalePlane);
    Vec3 delta = newMousePosition - mMousePosition;
    delta = referenceFrame.Conjugate().Rotate(delta);
    delta = Math::ComponentwiseProduct(axes, delta);
    if (snapping) {
      delta = Math::ScaleComponentsToInterval(delta, snapInterval);
    }
    mMousePosition += referenceFrame.Rotate(delta);
    return scale + delta;
  }
  else if (mOperation == Operation::Xyz) {
    Vec3 newMousePosition = Math::Intersection(mouseRay, mScalePlane);
    Vec3 mouseDelta = newMousePosition - mMousePosition;
    float amount = Math::Dot(mouseDelta, mUniformScaleDirection);
    mMousePosition += amount * mUniformScaleDirection;
    return scale + mNormalizedStartScale * amount;
  }
  return scale;
}

} // namespace Gizmos
} // namespace Editor
