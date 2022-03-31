#include "editor/gizmos/Translator.h"
#include "Input.h"
#include "comp/AlphaColor.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "editor/Editor.h"
#include "editor/gizmos/Gizmos.h"
#include "gfx/Renderer.h"
#include "math/Constants.h"

namespace Editor::Gizmos {

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
  // Create all of the handles.
  mParent = nSpace.CreateMember();
  nSpace.AddComponent<Comp::Transform>(mParent);
  for (int i = 0; i < smHandleCount; ++i) {
    mHandles[i] = nSpace.CreateChildMember(mParent);
    Comp::AlphaColor& alphaColorComp =
      nSpace.AddComponent<Comp::AlphaColor>(mHandles[i]);
    alphaColorComp.mAlphaColor = smHandleColors[i];
  }

  Comp::Transform& xT = nSpace.AddComponent<Comp::Transform>(mX);
  xT.SetTranslation({0.5f, 0.0f, 0.0f});
  Comp::Model& xM = nSpace.AddComponent<Comp::Model>(mX);
  xM.mModelId = AssLib::nArrowModelId;
  xM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& yT = nSpace.AddComponent<Comp::Transform>(mY);
  yT.SetTranslation({0.0f, 0.5f, 0.0f});
  Math::Quaternion yRotation;
  yRotation.AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  yT.SetRotation(yRotation);
  Comp::Model& yM = nSpace.AddComponent<Comp::Model>(mY);
  yM.mModelId = AssLib::nArrowModelId;
  yM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& zT = nSpace.AddComponent<Comp::Transform>(mZ);
  zT.SetTranslation({0.0f, 0.0f, 0.5f});
  Math::Quaternion zRotation;
  zRotation.AngleAxis(-Math::nPi / 2.0f, {0.0f, 1.0f, 0.0f});
  zT.SetRotation(zRotation);
  Comp::Model& zM = nSpace.AddComponent<Comp::Model>(mZ);
  zM.mModelId = AssLib::nArrowModelId;
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
  xyzT.SetUniformScale(0.15f);
  Comp::Model& xyzM = nSpace.AddComponent<Comp::Model>(mXyz);
  xyzM.mModelId = AssLib::nSphereModelId;
  xyzM.mShaderId = AssLib::nColorShaderId;
}

Translator::~Translator()
{
  nSpace.DeleteMember(mParent);
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
    Comp::AlphaColor& colorComp =
      *nSpace.GetComponent<Comp::AlphaColor>(mHandles[(int)mOperation]);
    colorComp.mAlphaColor = smHandleColors[(int)mOperation];
    mOperation = Operation::None;
    return translation;
  }
  if (Input::MousePressed(Input::Mouse::Left)) {
    SetNextOperation(translation, referenceFrame);
    if (mOperation == Operation::None) {
      return translation;
    }
    Editor::nSuppressObjectPicking |= true;
    Comp::AlphaColor& colorComp =
      *nSpace.GetComponent<Comp::AlphaColor>(mHandles[(int)mOperation]);
    colorComp.mAlphaColor = smActiveColor;
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

} // namespace Editor::Gizmos
