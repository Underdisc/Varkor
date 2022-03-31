#include "editor/gizmos/Scalor.h"
#include "Input.h"
#include "comp/AlphaColor.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "editor/Editor.h"
#include "editor/gizmos/Gizmos.h"
#include "gfx/Renderer.h"
#include "math/Constants.h"

namespace Editor::Gizmos {

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
  mParent = nSpace.CreateMember();
  nSpace.AddComponent<Comp::Transform>(mParent);
  for (int i = 0; i < smHandleCount; ++i) {
    mHandles[i] = nSpace.CreateChildMember(mParent);
    Comp::AlphaColor& colorComp =
      nSpace.AddComponent<Comp::AlphaColor>(mHandles[i]);
    colorComp.mAlphaColor = smHandleColors[i];
  }

  Comp::Transform& xT = nSpace.AddComponent<Comp::Transform>(mX);
  xT.SetTranslation({0.5f, 0.0f, 0.0f});
  Comp::Model& xM = nSpace.AddComponent<Comp::Model>(mX);
  xM.mModelId = AssLib::nScaleModelId;
  xM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& yT = nSpace.AddComponent<Comp::Transform>(mY);
  yT.SetTranslation({0.0f, 0.5f, 0.0f});
  Math::Quaternion yRotation;
  yRotation.AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  yT.SetRotation(yRotation);
  Comp::Model& yM = nSpace.AddComponent<Comp::Model>(mY);
  yM.mModelId = AssLib::nScaleModelId;
  yM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& zT = nSpace.AddComponent<Comp::Transform>(mZ);
  zT.SetTranslation({0.0f, 0.0f, 0.5f});
  Math::Quaternion zRotation;
  zRotation.AngleAxis(-Math::nPi / 2.0f, {0.0f, 1.0f, 0.0f});
  zT.SetRotation(zRotation);
  Comp::Model& zM = nSpace.AddComponent<Comp::Model>(mZ);
  zM.mModelId = AssLib::nScaleModelId;
  zM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& xyT = nSpace.AddComponent<Comp::Transform>(mXy);
  xyT.SetTranslation({0.5f, 0.5f, 0.0f});
  xyT.SetScale({0.15f, 0.15f, 0.01f});
  Comp::Model& xyM = nSpace.AddComponent<Comp::Model>(mXy);
  xyM.mModelId = AssLib::nCubeModelId;
  xyM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& xzT = nSpace.AddComponent<Comp::Transform>(mXz);
  xzT.SetTranslation({0.5f, 0.0f, 0.5f});
  xzT.SetScale({0.15f, 0.01f, 0.15f});
  Comp::Model& xzM = nSpace.AddComponent<Comp::Model>(mXz);
  xzM.mModelId = AssLib::nCubeModelId;
  xzM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& yzT = nSpace.AddComponent<Comp::Transform>(mYz);
  yzT.SetTranslation({0.0f, 0.5f, 0.5f});
  yzT.SetScale({0.01f, 0.15f, 0.15f});
  Comp::Model& yzM = nSpace.AddComponent<Comp::Model>(mYz);
  yzM.mModelId = AssLib::nCubeModelId;
  yzM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& xyzT = nSpace.AddComponent<Comp::Transform>(mXyz);
  xyzT.SetUniformScale(1.2f);
  Comp::Model& xyzM = nSpace.AddComponent<Comp::Model>(mXyz);
  xyzM.mModelId = AssLib::nTorusModelId;
  xyzM.mShaderId = AssLib::nColorShaderId;
}

Scalor::~Scalor()
{
  nSpace.DeleteMember(mParent);
}

void Scalor::SetNextOperation(
  const Vec3& scale, const Vec3& translation, const Quat& referenceFrame)
{
  // Find the next operation type.
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

  // Set values to be used for the selected operation during following frames.
  mScaleRay.mStart = translation;
  mScalePlane.mPoint = translation;
  Vec3 xRotated = referenceFrame.Rotate({1.0f, 0.0f, 0.0f});
  Vec3 yRotated = referenceFrame.Rotate({0.0f, 1.0f, 0.0f});
  Vec3 zRotated = referenceFrame.Rotate({0.0f, 0.0f, 1.0f});
  switch (mOperation) {
  case Operation::X: mScaleRay.Direction(xRotated); break;
  case Operation::Y: mScaleRay.Direction(yRotated); break;
  case Operation::Z: mScaleRay.Direction(zRotated); break;
  case Operation::Xy: mScalePlane.Normal(zRotated); break;
  case Operation::Xz: mScalePlane.Normal(yRotated); break;
  case Operation::Yz: mScalePlane.Normal(xRotated); break;
  case Operation::Xyz: mScalePlane.Normal(nCamera.Forward()); break;
  }
  Math::Ray mouseRay =
    nCamera.StandardPositionToRay(Input::StandardMousePosition());
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
  Vec3 torusDirection = nCamera.Position() - translation;
  Math::Quaternion xyzRotation;
  xyzRotation.FromTo({1.0f, 0.0f, 0.0f}, torusDirection);
  xyzRotation = referenceFrame.Conjugate() * xyzRotation;
  Comp::Transform& xyzTransform = *nSpace.GetComponent<Comp::Transform>(mXyz);
  xyzTransform.SetRotation(xyzRotation);

  // Handle any switching between operations.
  if (!Input::MouseDown(Input::Mouse::Left) && mOperation != Operation::None) {
    Comp::AlphaColor& colorComp =
      *nSpace.GetComponent<Comp::AlphaColor>(mHandles[(int)mOperation]);
    colorComp.mAlphaColor = smHandleColors[(int)mOperation];
    mOperation = Operation::None;
    return scale;
  }
  if (Input::MousePressed(Input::Mouse::Left)) {
    SetNextOperation(scale, translation, referenceFrame);
    if (mOperation == Operation::None) {
      return scale;
    }
    Editor::nSuppressObjectPicking |= true;
    Comp::AlphaColor& colorComp =
      *nSpace.GetComponent<Comp::AlphaColor>(mHandles[(int)mOperation]);
    colorComp.mAlphaColor = smActiveColor;
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
  }
  Math::Ray mouseRay =
    nCamera.StandardPositionToRay(Input::StandardMousePosition());
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

} // namespace Editor::Gizmos
