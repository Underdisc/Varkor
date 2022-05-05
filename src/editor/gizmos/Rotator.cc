#include "editor/gizmos/Rotator.h"
#include "Input.h"
#include "comp/AlphaColor.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "editor/Editor.h"
#include "editor/gizmos/Gizmos.h"
#include "gfx/Renderer.h"
#include "math/Constants.h"

namespace Editor::Gizmos {

Quat Rotate(
  const Quat& rotation,
  const Vec3& translation,
  const Quat& referenceFrame,
  bool snapping,
  float snapInterval)
{
  return GetInstance<Rotator>()->Run(
    rotation, translation, referenceFrame, snapping, snapInterval);
}

Rotator::Rotator(): mOperation(Operation::None)
{
  // Create all of the handles.
  mParent = nSpace.CreateMember();
  nSpace.AddComponent<Comp::Transform>(mParent);
  for (int i = 0; i < smHandleCount; ++i) {
    mHandles[i] = nSpace.CreateChildMember(mParent);
    Comp::AlphaColor& alphaColorComp =
      nSpace.AddComponent<Comp::AlphaColor>(mHandles[i]);
    alphaColorComp.mColor = smHandleColors[i];
  }

  nSpace.AddComponent<Comp::Transform>(mX);
  Comp::Model& xM = nSpace.AddComponent<Comp::Model>(mX);
  xM.mModelId = AssLib::nTorusModelId;
  xM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& yT = nSpace.AddComponent<Comp::Transform>(mY);
  Comp::Model& yM = nSpace.AddComponent<Comp::Model>(mY);
  Math::Quaternion rotation;
  rotation.AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  yT.SetRotation(rotation);
  yM.mModelId = AssLib::nTorusModelId;
  yM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& zT = nSpace.AddComponent<Comp::Transform>(mZ);
  Comp::Model& zM = nSpace.AddComponent<Comp::Model>(mZ);
  rotation.AngleAxis(-Math::nPi / 2.0f, {0.0f, 1.0f, 0.0f});
  zT.SetRotation(rotation);
  zM.mModelId = AssLib::nTorusModelId;
  zM.mShaderId = AssLib::nColorShaderId;

  Comp::Transform& xyzT = nSpace.AddComponent<Comp::Transform>(mXyz);
  Comp::Model& xyzM = nSpace.AddComponent<Comp::Model>(mXyz);
  xyzT.SetUniformScale(0.8f);
  xyzM.mModelId = AssLib::nSphereModelId;
  xyzM.mShaderId = AssLib::nColorShaderId;
}

Rotator::~Rotator()
{
  nSpace.DeleteMember(mParent);
}

void Rotator::SetNextOperation(
  const Vec3& translation, const Quat& referenceFrame)
{
  // Find out the type of operation.
  World::MemberId hoveredHandleId =
    Gfx::Renderer::HoveredMemberId(nSpace, nCamera.View(), nCamera.Proj());
  // clang-format off
  if (hoveredHandleId == mX) { mOperation = Operation::X; }
  else if (hoveredHandleId == mY) { mOperation = Operation::Y; }
  else if (hoveredHandleId == mZ) { mOperation = Operation::Z; }
  else if (hoveredHandleId == mXyz) { mOperation = Operation::Xyz; }
  else { mOperation = Operation::None; return; }
  // clang-format on

  // Prepare mRotationPlane for the new operation.
  mRotationPlane.mPoint = translation;
  Vec3 xRotated = referenceFrame.Rotate({1.0f, 0.0f, 0.0f});
  Vec3 yRotated = referenceFrame.Rotate({0.0f, 1.0f, 0.0f});
  Vec3 zRotated = referenceFrame.Rotate({0.0f, 0.0f, 1.0f});
  switch (mOperation) {
  case Operation::X: mRotationPlane.Normal(xRotated); break;
  case Operation::Y: mRotationPlane.Normal(yRotated); break;
  case Operation::Z: mRotationPlane.Normal(zRotated); break;
  }

  // Prepare mMouseOffset for the new operation.
  Math::Ray mouseRay =
    nCamera.StandardPositionToRay(Input::StandardMousePosition());
  bool singleAxisOperation = mOperation != Operation::Xyz;
  if (singleAxisOperation && Math::HasIntersection(mouseRay, mRotationPlane)) {
    mMouseOffset = Math::Intersection(mouseRay, mRotationPlane) - translation;
    if (Math::Near(mMouseOffset, {0.0f, 0.0f, 0.0f})) {
      mOperation = Operation::None;
    }
  }
}

Quat Rotator::Run(
  const Quat& rotation,
  const Vec3& translation,
  const Quat& referenceFrame,
  bool snapping,
  float snapInterval)
{
  SetParentTransformation(mParent, translation, referenceFrame);

  // Handle the transitions between all operation types.
  if (!Input::MouseDown(Input::Mouse::Left) && mOperation != Operation::None) {
    Comp::AlphaColor& alphaColorComp =
      nSpace.GetComponent<Comp::AlphaColor>(mHandles[(int)mOperation]);
    alphaColorComp.mColor = smHandleColors[(int)mOperation];
    mOperation = Operation::None;
    return rotation;
  }
  if (Input::MousePressed(Input::Mouse::Left)) {
    SetNextOperation(translation, referenceFrame);
    if (mOperation == Operation::None) {
      return rotation;
    }
    Editor::nSuppressObjectPicking |= true;
    Comp::AlphaColor& alphaColorComp =
      nSpace.GetComponent<Comp::AlphaColor>(mHandles[(int)mOperation]);
    alphaColorComp.mColor = smActiveColor;
    if (mOperation == Operation::Xyz) {
      alphaColorComp.mColor[3] = 0.8f;
    }
    return rotation;
  }

  // Perform the active operation.
  if (mOperation < Operation::Xyz) {
    // We get two rays on the rotation plane and find the angle between them to
    // determine the change in rotation.
    Math::Ray mouseRay =
      nCamera.StandardPositionToRay(Input::StandardMousePosition());
    if (!Math::HasIntersection(mouseRay, mRotationPlane)) {
      return rotation;
    }
    Vec3 newMouseOffset = Math::Intersection(mouseRay, mRotationPlane);
    newMouseOffset -= translation;
    if (Math::Near(Math::MagnitudeSq(newMouseOffset), 0.0f)) {
      return rotation;
    }
    Vec3 normOld = Math::Normalize(mMouseOffset);
    Vec3 normNew = Math::Normalize(newMouseOffset);
    // We need to clamp to avoid nan values from the cosine function.
    float dot = Math::Clamp(-1.0f, 1.0f, Math::Dot(normOld, normNew));
    float angle = std::acosf(dot);
    if (snapping) {
      angle = Math::RoundToNearest(angle, snapInterval);
    }
    Vec3 positiveDirection = Math::Cross(mRotationPlane.Normal(), normOld);
    if (Math::Dot(normNew, positiveDirection) < 0.0f) {
      angle *= -1.0f;
    }
    Math::Quaternion delta;
    delta.AngleAxis(angle, mRotationPlane.Normal());
    mMouseOffset = delta.Rotate(mMouseOffset);
    return delta * rotation;
  }
  else if (mOperation == Operation::Xyz) {
    const float pixelsPerRadian = 500.0f;
    Vec2 radians = (Input::MouseMotion() / pixelsPerRadian) * Math::nPi;
    Math::Quaternion horizontalRotation, verticalRotation;
    horizontalRotation.AngleAxis(radians[0], nCamera.Up());
    verticalRotation.AngleAxis(radians[1], nCamera.Right());
    return horizontalRotation * verticalRotation * rotation;
  }
  return rotation;
}

} // namespace Editor::Gizmos
