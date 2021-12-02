#include <glad/glad.h>
#include <imgui/imgui.h>

#include "Input.h"
#include "Viewport.h"
#include "comp/Model.h"
#include "editor/Editor.h"
#include "editor/Utility.h"
#include "editor/hook/Transform.h"
#include "gfx/Model.h"
#include "gfx/Renderer.h"
#include "math/Constants.h"
#include "math/Geometry.h"
#include "world/World.h"

namespace Editor {

Hook<Comp::Transform>::Hook():
  mDrawbuffer(GL_RGBA, GL_UNSIGNED_BYTE),
  mReferenceFrame(ReferenceFrame::World),
  mSnapping(false),
  mTranslateSnapInterval(1.0f),
  mScaleSnapInterval(0.1f),
  mRotationSnapDenominator(4),
  mOperation(Operation::None)
{
  mParent = mSpace.CreateMember();
  mX = mSpace.CreateChildMember(mParent);
  mY = mSpace.CreateChildMember(mParent);
  mZ = mSpace.CreateChildMember(mParent);
  mXy = mSpace.CreateChildMember(mParent);
  mXz = mSpace.CreateChildMember(mParent);
  mYz = mSpace.CreateChildMember(mParent);
  mXyz = mSpace.CreateChildMember(mParent);

  // Add all of the components needed for the gizmo's handles.
  Comp::Transform& parentT = mSpace.AddComponent<Comp::Transform>(mParent);
  Comp::Transform& xT = mSpace.AddComponent<Comp::Transform>(mX);
  Comp::Transform& yT = mSpace.AddComponent<Comp::Transform>(mY);
  Comp::Transform& zT = mSpace.AddComponent<Comp::Transform>(mZ);
  Comp::Transform& xyT = mSpace.AddComponent<Comp::Transform>(mXy);
  Comp::Transform& xzT = mSpace.AddComponent<Comp::Transform>(mXz);
  Comp::Transform& yzT = mSpace.AddComponent<Comp::Transform>(mYz);
  Comp::Transform& xyzT = mSpace.AddComponent<Comp::Transform>(mXyz);
  Comp::Model& xM = mSpace.AddComponent<Comp::Model>(mX);
  Comp::Model& yM = mSpace.AddComponent<Comp::Model>(mY);
  Comp::Model& zM = mSpace.AddComponent<Comp::Model>(mZ);
  Comp::Model& xyM = mSpace.AddComponent<Comp::Model>(mXy);
  Comp::Model& xzM = mSpace.AddComponent<Comp::Model>(mXz);
  Comp::Model& yzM = mSpace.AddComponent<Comp::Model>(mYz);
  Comp::Model& xyzM = mSpace.AddComponent<Comp::Model>(mXyz);

  // Initialize the transform and model components of the handles so that the
  // gizmo begins as a translate gizmo.
  mMode = Mode::Translate;
  xT.SetTranslation({0.5f, 0.0f, 0.0f});
  yT.SetTranslation({0.0f, 0.5f, 0.0f});
  zT.SetTranslation({0.0f, 0.0f, 0.5f});
  Math::Quaternion yRotation, zRotation;
  yRotation.AngleAxis(Math::nPi / 2.0f, {0.0f, 0.0f, 1.0f});
  zRotation.AngleAxis(-Math::nPi / 2.0f, {0.0f, 1.0f, 0.0f});
  yT.SetRotation(yRotation);
  zT.SetRotation(zRotation);
  xyT.SetTranslation({0.5f, 0.5f, 0.0f});
  xyT.SetScale({0.15f, 0.15f, 0.01f});
  xzT.SetTranslation({0.5f, 0.0f, 0.5f});
  xzT.SetScale({0.15f, 0.01f, 0.15f});
  yzT.SetTranslation({0.0f, 0.5f, 0.5f});
  yzT.SetScale({0.01f, 0.15f, 0.15f});
  xyzT.SetUniformScale(0.15f);
  xM.mModelId = AssLib::nArrowModelId;
  yM.mModelId = AssLib::nArrowModelId;
  zM.mModelId = AssLib::nArrowModelId;
  xyM.mModelId = AssLib::nCubeModelId;
  xzM.mModelId = AssLib::nCubeModelId;
  yzM.mModelId = AssLib::nCubeModelId;
  xyzM.mModelId = AssLib::nSphereModelId;
}

bool Hook<Comp::Transform>::Edit(const World::Object& object)
{
  Comp::Transform* transform = object.GetComponent<Comp::Transform>();
  Vec3 translation = transform->GetTranslation();
  ImGui::PushItemWidth(-60.0f);
  ImGui::DragFloat3("Position", translation.mD, 0.01f);
  transform->SetTranslation(translation);

  Vec3 scale = transform->GetScale();
  ImGui::DragFloat3("Scale", scale.mD, 0.01f);
  transform->SetScale(scale);

  Quat rotation = transform->GetRotation();
  Vec3 eulerAngles = rotation.EulerAngles();
  Vec3 newAngles = eulerAngles;
  bool rotationDragged = ImGui::DragFloat3(
    "Rotation",
    newAngles.mD,
    0.01f,
    0.0f,
    0.0f,
    "%.3f",
    ImGuiSliderFlags_NoInput);
  if (rotationDragged)
  {
    Vec3 delta = newAngles - eulerAngles;
    Math::Quaternion xDelta, yDelta, zDelta;
    xDelta.AngleAxis(delta[0], {1.0f, 0.0f, 0.0f});
    yDelta.AngleAxis(delta[1], {0.0f, 1.0f, 0.0f});
    zDelta.AngleAxis(delta[2], {0.0f, 0.0f, 1.0f});
    transform->SetRotation(zDelta * yDelta * xDelta * rotation);
  }
  ImGui::PopItemWidth();
  return Gizmo(transform, object);
}

bool Hook<Comp::Transform>::Gizmo(
  Comp::Transform* transform, const World::Object& object)
{
  if (Input::KeyDown(Input::Key::LeftControl))
  {
    if (Input::KeyPressed(Input::Key::Z))
    {
      mReferenceFrame = ReferenceFrame::World;
    } else if (Input::KeyPressed(Input::Key::X))
    {
      mReferenceFrame = ReferenceFrame::Parent;
    } else if (Input::KeyPressed(Input::Key::C))
    {
      mReferenceFrame = ReferenceFrame::Relative;
    }
  } else
  {
    if (Input::KeyPressed(Input::Key::Z))
    {
      SwitchMode(Mode::Translate);
    } else if (Input::KeyPressed(Input::Key::X))
    {
      SwitchMode(Mode::Scale);
    } else if (Input::KeyPressed(Input::Key::C))
    {
      SwitchMode(Mode::Rotate);
    }
  }
  DisplayOptionsWindow();

  // Find the world ray represented by the mouse and camera positions.
  Vec3 worldPosition = Viewport::MouseToWorldPosition(
    Input::StandardMousePosition(), nCamera.CameraToWorld());
  const Vec3& cameraPosition = nCamera.Position();
  Math::Ray mouseRay;
  mouseRay.StartDirection(cameraPosition, worldPosition - cameraPosition);
  const World::Space& space = World::GetSpace(object.mSpace);

  // Check to see if the user is beginning or ending a gizmo operation.
  if (Input::MousePressed(Input::Mouse::Left))
  {
    TryStartOperation(transform, space, object.mMember, mouseRay);
  }
  if (!Input::MouseDown(Input::Mouse::Left))
  {
    mOperation = Operation::None;
  }

  // Perform the current operation and render the gizmo's handles.
  bool noObjectPicking = false;
  switch (mMode)
  {
  case Mode::Translate:
    noObjectPicking = Translate(transform, space, object.mMember, mouseRay);
    break;
  case Mode::Scale:
    noObjectPicking = Scale(transform, space, object.mMember, mouseRay);
    break;
  case Mode::Rotate:
    noObjectPicking = Rotate(transform, space, object.mMember, mouseRay);
    break;
  }
  RenderHandles(transform, space, object.mMember);
  return noObjectPicking;
}

void Hook<Comp::Transform>::DisplayOptionsWindow()
{
  ImGui::Begin("Transform Gizmo Options");
  ImGui::PushItemWidth(-110.0f);
  const char* modeNames[] = {"Translate", "Scale", "Rotate"};
  const int modeNameCount = sizeof(modeNames) / sizeof(const char*);
  int intMode = (int)mMode;
  ImGui::Combo("Mode", &intMode, modeNames, modeNameCount);
  Mode newMode = (Mode)intMode;
  if (newMode != mMode)
  {
    SwitchMode(newMode);
  }
  const char* frameNames[] = {"World", "Parent", "Relative"};
  const int frameNameCount = sizeof(frameNames) / sizeof(const char*);
  int intFrame = (int)mReferenceFrame;
  ImGui::Combo("Reference Frame", &intFrame, frameNames, frameNameCount);
  mReferenceFrame = (ReferenceFrame)intFrame;

  ImGui::Checkbox("Snapping", &mSnapping);
  if (mSnapping)
  {
    ImGui::Text("Snap Parameters");
    ImGui::InputFloat("Translate", &mTranslateSnapInterval, 0.1f);
    ImGui::InputFloat("Scale", &mScaleSnapInterval, 0.1f);
    ImGui::InputInt("Rotation", &mRotationSnapDenominator);
    HelpMarker("This represents the number of snaps until 180 degrees.");
  }

  ImGui::PopItemWidth();
  ImGui::End();
}

void Hook<Comp::Transform>::TryStartOperation(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId,
  const Math::Ray& mouseRay)
{
  // Determine if the user began an operation.
  World::MemberId handleId =
    Gfx::Renderer::HoveredMemberId(mSpace, nCamera.WorldToCamera());
  mOperation = GetHandleOperation(handleId);
  if (mOperation == Operation::None)
  {
    return;
  }

  // Save values related to the user's state when starting an operation.
  mStartScale = transform->GetScale();
  mStartRotation = transform->GetRotation();
  mStartWorldRotation = transform->GetWorldRotation(space, ownerId);
  Math::Ray gizmoRay;
  Math::Plane gizmoPlane;
  PrepareGizmoRepresentation(transform, space, ownerId, &gizmoRay, &gizmoPlane);
  Vec3 worldTranslation = transform->GetWorldTranslation(space, ownerId);
  switch (mOperation)
  {
  case Operation::X:
  case Operation::Y:
  case Operation::Z:
    if (mMode == Mode::Rotate && Math::HasIntersection(mouseRay, gizmoPlane))
    {
      Vec3 intersection = Math::Intersection(mouseRay, gizmoPlane);
      mTranslateOffset = intersection - worldTranslation;
    } else if (gizmoRay.HasClosestTo(mouseRay))
    {
      mTranslateOffset = gizmoRay.ClosestPointTo(mouseRay) - worldTranslation;
    } else
    {
      mOperation = Operation::None;
    }
    break;
  case Operation::Xy:
  case Operation::Xz:
  case Operation::Yz:
  case Operation::Xyz:
    if (Math::HasIntersection(mouseRay, gizmoPlane))
    {
      Vec3 intersection = Math::Intersection(mouseRay, gizmoPlane);
      mTranslateOffset = intersection - worldTranslation;
    } else
    {
      mOperation = Operation::None;
    }
    break;
  }
}

bool Hook<Comp::Transform>::Translate(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId,
  const Math::Ray& mouseRay)
{
  Math::Ray gizmoRay;
  Math::Plane gizmoPlane;
  Vec3 planeAxis;
  PrepareGizmoRepresentation(
    transform, space, ownerId, &gizmoRay, &gizmoPlane, &planeAxis);

  auto singleTranslate = [&]()
  {
    if (!gizmoRay.HasClosestTo(mouseRay))
    {
      return;
    }
    Vec3 mousePoint = gizmoRay.ClosestPointTo(mouseRay);
    Vec3 newWorldTranslation = mousePoint - mTranslateOffset;
    if (!mSnapping)
    {
      transform->SetWorldTranslation(newWorldTranslation, space, ownerId);
      return;
    }
    Vec3 translation = transform->GetTranslation();
    Vec3 newTranslation =
      transform->WorldToLocalTranslation(newWorldTranslation, space, ownerId);
    Vec3 delta = newTranslation - translation;
    delta = ScaleToInterval(delta, mTranslateSnapInterval);
    newTranslation = translation + delta;
    transform->SetTranslation(newTranslation);
  };

  auto multiTranslate = [&]()
  {
    if (!Math::HasIntersection(mouseRay, gizmoPlane))
    {
      return;
    }
    Vec3 mousePoint = Math::Intersection(mouseRay, gizmoPlane);
    Vec3 newWorldTranslation = mousePoint - mTranslateOffset;
    if (!mSnapping)
    {
      transform->SetWorldTranslation(newWorldTranslation, space, ownerId);
      return;
    }
    Vec3 translation = transform->GetTranslation();
    Vec3 newTranslation =
      transform->WorldToLocalTranslation(newWorldTranslation, space, ownerId);
    Vec3 delta = newTranslation - translation;
    // We find perpendicular vectors that describe the delta and are parallel to
    // the plane that the transform is moving on.
    Quat pWorldRotation = transform->GetParentWorldRotation(space, ownerId);
    planeAxis = pWorldRotation.Conjugate().Rotate(planeAxis);
    Math::Ray planeRay;
    planeRay.StartDirection({0.0f, 0.0f, 0.0f}, planeAxis);
    Vec3 axesDelta[2];
    axesDelta[0] = planeRay.ClosestPointTo(delta);
    axesDelta[1] = delta - axesDelta[0];
    // We then scale those vectors to the snap interval to find the snap delta.
    axesDelta[0] = ScaleToInterval(axesDelta[0], mTranslateSnapInterval);
    axesDelta[1] = ScaleToInterval(axesDelta[1], mTranslateSnapInterval);
    newTranslation = translation + axesDelta[0] + axesDelta[1];
    transform->SetTranslation(newTranslation);
  };

  switch (mOperation)
  {
  case Operation::X:
  case Operation::Y:
  case Operation::Z: singleTranslate(); return true;
  case Operation::Xy:
  case Operation::Xz:
  case Operation::Yz: multiTranslate(); return true;
  case Operation::Xyz:
    if (Math::HasIntersection(mouseRay, gizmoPlane))
    {
      Vec3 mousePoint = Math::Intersection(mouseRay, gizmoPlane);
      Vec3 newWorldTranslation = mousePoint - mTranslateOffset;
      transform->SetWorldTranslation(newWorldTranslation, space, ownerId);
    }
    return true;
  }
  return false;
}

bool Hook<Comp::Transform>::Scale(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId,
  const Math::Ray& mouseRay)
{
  Math::Ray gizmoRay;
  Math::Plane gizmoPlane;
  PrepareGizmoRepresentation(transform, space, ownerId, &gizmoRay, &gizmoPlane);
  Vec3 startingPoint =
    transform->GetWorldTranslation(space, ownerId) + mTranslateOffset;
  const float sensitivity = 0.3f;

  // This will peform scaling on a single axis.
  auto singleScale = [&](const Vec3& axis)
  {
    if (!gizmoRay.HasClosestTo(mouseRay))
    {
      return;
    }
    float mouseT = gizmoRay.ClosestTTo(mouseRay);
    float startT = gizmoRay.ClosestTTo(startingPoint);
    float deltaT = mouseT - startT;
    float deltaScaler = deltaT * sensitivity;
    if (!mSnapping)
    {
      transform->SetScale(mStartScale + axis * deltaScaler);
      return;
    }
    deltaScaler = Math::RoundToNearest(deltaScaler, mScaleSnapInterval);
    transform->SetScale(mStartScale + axis * deltaScaler);
  };

  // This will perform scaling on multiple axes.
  auto multiScale = [&](const Vec3& axes)
  {
    if (!Math::HasIntersection(mouseRay, gizmoPlane))
    {
      return;
    }
    Vec3 mousePoint = Math::Intersection(mouseRay, gizmoPlane);
    Vec3 mouseDistance = mousePoint - startingPoint;
    Vec3 offsetDirection = Math::Normalize(mTranslateOffset);
    float deltaScaler = Math::Dot(offsetDirection, mouseDistance) * sensitivity;
    if (!mSnapping)
    {
      Vec3 ratios;
      if (Math::Near(mStartScale, {0.0f, 0.0f, 0.0f}))
      {
        ratios = axes;
      } else
      {
        ratios = Math::ComponentwiseProduct(mStartScale, axes);
      }
      transform->SetScale(mStartScale + ratios * deltaScaler);
      return;
    }
    deltaScaler = Math::RoundToNearest(deltaScaler, mScaleSnapInterval);
    transform->SetScale(mStartScale + axes * deltaScaler);
  };

  switch (mOperation)
  {
  case Operation::X: singleScale({1.0, 0.0f, 0.0f}); return true;
  case Operation::Y: singleScale({0.0, 1.0f, 0.0f}); return true;
  case Operation::Z: singleScale({0.0, 0.0f, 1.0f}); return true;
  case Operation::Xy: multiScale({1.0, 1.0f, 0.0f}); return true;
  case Operation::Xz: multiScale({1.0, 0.0f, 1.0f}); return true;
  case Operation::Yz: multiScale({0.0, 1.0f, 1.0f}); return true;
  case Operation::Xyz: multiScale({1.0f, 1.0f, 1.0f}); return true;
  }
  return false;
}

bool Hook<Comp::Transform>::Rotate(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId,
  const Math::Ray& mouseRay)
{
  Math::Ray gizmoRay;
  Math::Plane gizmoPlane;
  PrepareGizmoRepresentation(transform, space, ownerId, &gizmoRay, &gizmoPlane);

  // This will perform a rotation around a single axis.
  auto singleRotate = [&]()
  {
    if (!Math::HasIntersection(mouseRay, gizmoPlane))
    {
      return;
    }
    Vec3 currentPoint = Math::Intersection(mouseRay, gizmoPlane);
    Vec3 worldTranslation = transform->GetWorldTranslation(space, ownerId);
    Vec3 currentOffset = currentPoint - worldTranslation;
    // We don't need to check if the magnitude of mTranslateOffset is 0 because
    // it never should be.
    if (Math::Near(Math::MagnitudeSq(currentOffset), 0.0f))
    {
      return;
    }
    Vec3 normCurrent = Math::Normalize(currentOffset);
    Vec3 normStart = Math::Normalize(mTranslateOffset);
    // We need to clamp to avoid nan values from the cosine function.
    float scDot = Math::Clamp(-1.0f, 1.0f, Math::Dot(normStart, normCurrent));
    float angle = std::acosf(scDot);
    Vec3 positiveDirection = Math::Cross(gizmoPlane.Normal(), normStart);
    if (Math::Dot(normCurrent, positiveDirection) < 0.0f)
    {
      angle *= -1.0f;
    }
    if (mSnapping)
    {
      float snapDelta = Math::nPi / (float)mRotationSnapDenominator;
      angle = Math::RoundToNearest(angle, snapDelta);
    }
    Math::Quaternion parentRotation =
      transform->GetParentWorldRotation(space, ownerId);
    Vec3 axis = parentRotation.Conjugate().Rotate(gizmoPlane.Normal());
    Math::Quaternion delta;
    delta.AngleAxis(angle, axis);
    transform->SetRotation(delta * mStartRotation);
  };

  // This will perform a rotation around multiple axes using the mouse motion.
  auto multiRotate = [&]()
  {
    Vec2 mouseMotion = Input::MouseMotion();
    const float pixelsPerRadian = 500.0f;
    mouseMotion = (mouseMotion / pixelsPerRadian) * Math::nPi;
    Math::Quaternion horizontalRotation, verticalRotation;
    Math::Quaternion parentRotation =
      transform->GetParentWorldRotation(space, ownerId);
    Vec3 up = parentRotation.Conjugate().Rotate(nCamera.Up());
    Vec3 right = parentRotation.Conjugate().Rotate(nCamera.Right());
    horizontalRotation.AngleAxis(mouseMotion[0], up);
    verticalRotation.AngleAxis(mouseMotion[1], right);
    transform->SetRotation(
      horizontalRotation * verticalRotation * transform->GetRotation());
  };

  switch (mOperation)
  {
  case Operation::X:
  case Operation::Y:
  case Operation::Z: singleRotate(); return true;
  case Operation::Xyz: multiRotate(); return true;
  }
  return false;
}

void Hook<Comp::Transform>::SwitchMode(Mode newMode)
{
  Comp::Transform* xT = mSpace.GetComponent<Comp::Transform>(mX);
  Comp::Transform* yT = mSpace.GetComponent<Comp::Transform>(mY);
  Comp::Transform* zT = mSpace.GetComponent<Comp::Transform>(mZ);
  Comp::Transform* xyzT = mSpace.GetComponent<Comp::Transform>(mXyz);
  Comp::Model* xM = mSpace.GetComponent<Comp::Model>(mX);
  Comp::Model* yM = mSpace.GetComponent<Comp::Model>(mY);
  Comp::Model* zM = mSpace.GetComponent<Comp::Model>(mZ);
  Comp::Model* xyzM = mSpace.GetComponent<Comp::Model>(mXyz);
  mMode = newMode;
  switch (mMode)
  {
  case Mode::Translate:
    xT->SetTranslation({0.5f, 0.0f, 0.0f});
    yT->SetTranslation({0.0f, 0.5f, 0.0f});
    zT->SetTranslation({0.0f, 0.0f, 0.5f});
    xyzT->SetUniformScale(0.15f);
    xM->mModelId = AssLib::nArrowModelId;
    yM->mModelId = AssLib::nArrowModelId;
    zM->mModelId = AssLib::nArrowModelId;
    xyzM->mModelId = AssLib::nSphereModelId;
    break;
  case Mode::Scale:
    xT->SetTranslation({0.5f, 0.0f, 0.0f});
    yT->SetTranslation({0.0f, 0.5f, 0.0f});
    zT->SetTranslation({0.0f, 0.0f, 0.5f});
    xyzT->SetUniformScale(1.1f);
    xM->mModelId = AssLib::nScaleModelId;
    yM->mModelId = AssLib::nScaleModelId;
    zM->mModelId = AssLib::nScaleModelId;
    xyzM->mModelId = AssLib::nTorusModelId;
    break;
  case Mode::Rotate:
    xT->SetTranslation({0.0f, 0.0f, 0.0f});
    yT->SetTranslation({0.0f, 0.0f, 0.0f});
    zT->SetTranslation({0.0f, 0.0f, 0.0f});
    xyzT->SetUniformScale(0.8f);
    xM->mModelId = AssLib::nTorusModelId;
    yM->mModelId = AssLib::nTorusModelId;
    zM->mModelId = AssLib::nTorusModelId;
    xyzM->mModelId = AssLib::nSphereModelId;
  }
}

Hook<Comp::Transform>::Operation Hook<Comp::Transform>::GetHandleOperation(
  World::MemberId handleId) const
{
  // clang-format off
  if (handleId == mX) { return Operation::X; }
  else if (handleId == mY) { return Operation::Y; }
  else if (handleId == mZ) { return Operation::Z; }
  else if (handleId == mXy) { return Operation::Xy; }
  else if (handleId == mXz) { return Operation::Xz; }
  else if (handleId == mYz) { return Operation::Yz; }
  else if (handleId == mXyz) { return Operation::Xyz; }
  return Operation::None;
  // clang-format on
}

void Hook<Comp::Transform>::PrepareGizmoRepresentation(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId,
  Math::Ray* gizmoRay,
  Math::Plane* gizmoPlane,
  Vec3* planeAxis) const
{
  // Place the gizmo representations in the correct position.
  Mat4 worldMatrix = transform->GetWorldMatrix(space, ownerId);
  Vec3 worldTranslation = Math::ApplyToPoint(worldMatrix, {0.0f, 0.0f, 0.0f});
  gizmoRay->mStart = worldTranslation;
  gizmoPlane->mPoint = worldTranslation;

  // Rotate the gizmo representations to the correct orientation.
  Math::Quaternion rotation = ReferenceFrameRotation(transform, space, ownerId);
  if (mReferenceFrame == ReferenceFrame::Relative)
  {
    // The reference frame function returned the transform's current world
    // rotation. This creates an unstable quaternion when rotating around a
    // single axis. We overwrite the rotation here to keep this quaternion the
    // same throughout an entire operation.
    rotation = mStartWorldRotation;
  }
  Vec3 x = rotation.Rotate({1.0f, 0.0f, 0.0f});
  Vec3 y = rotation.Rotate({0.0f, 1.0f, 0.0f});
  Vec3 z = rotation.Rotate({0.0f, 0.0f, 1.0f});
  switch (mMode)
  {
  case Mode::Translate:
  case Mode::Scale:
    switch (mOperation)
    {
    case Operation::X: gizmoRay->Direction(x); break;
    case Operation::Y: gizmoRay->Direction(y); break;
    case Operation::Z: gizmoRay->Direction(z); break;
    case Operation::Xy: gizmoPlane->Normal(z); break;
    case Operation::Xz: gizmoPlane->Normal(y); break;
    case Operation::Yz: gizmoPlane->Normal(x); break;
    case Operation::Xyz: gizmoPlane->Normal(nCamera.Forward()); break;
    }
    break;
  case Mode::Rotate:
    switch (mOperation)
    {
    case Operation::X: gizmoPlane->Normal(x); break;
    case Operation::Y: gizmoPlane->Normal(y); break;
    case Operation::Z: gizmoPlane->Normal(z); break;
    }
    break;
  }

  if (planeAxis == nullptr)
  {
    return;
  }
  switch (mOperation)
  {
  case Operation::Xy: *planeAxis = x; break;
  case Operation::Xz: *planeAxis = x; break;
  case Operation::Yz: *planeAxis = y; break;
  }
}

Quat Hook<Comp::Transform>::ReferenceFrameRotation(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId) const
{
  Quat rotation;
  if (mMode == Mode::Scale)
  {
    rotation = transform->GetWorldRotation(space, ownerId);
    return rotation;
  }
  switch (mReferenceFrame)
  {
  case ReferenceFrame::World: rotation.Identity(); break;
  case ReferenceFrame::Parent:
    rotation = transform->GetParentWorldRotation(space, ownerId);
    break;
  case ReferenceFrame::Relative:
    rotation = transform->GetWorldRotation(space, ownerId);
    break;
  }
  return rotation;
}

Vec3 Hook<Comp::Transform>::ScaleToInterval(Vec3 vector, float interval)
{
  float length = Math::Magnitude(vector);
  if (Math::Near(length, 0.0f))
  {
    return {0.0f, 0.0f, 0.0f};
  }
  float newLength = Math::RoundToNearest(length, interval);
  vector = vector / length;
  return vector * newLength;
}

void Hook<Comp::Transform>::RenderHandle(
  World::MemberId handleId, const Vec4& color)
{
  const Gfx::Shader& colorShader =
    AssLib::Get<Gfx::Shader>(AssLib::nColorShaderId);
  colorShader.SetVec4("uColor", color.CData());

  Comp::Transform& transform = *mSpace.GetComponent<Comp::Transform>(handleId);
  Mat4 memberTransformation = transform.GetWorldMatrix(mSpace, handleId);
  Comp::Model& modelComp = *mSpace.GetComponent<Comp::Model>(handleId);
  const Gfx::Model& model = AssLib::Get<Gfx::Model>(modelComp.mModelId);
  const Ds::Vector<Gfx::Model::DrawInfo>& allDrawInfo = model.GetAllDrawInfo();
  for (const Gfx::Model::DrawInfo& drawInfo : allDrawInfo)
  {
    Mat4 transformation = memberTransformation * drawInfo.mTransformation;
    colorShader.SetMat4("uModel", transformation.CData());
    const Gfx::Mesh& mesh = model.GetMesh(drawInfo.mMeshIndex);
    glBindVertexArray(mesh.Vao());
    glDrawElements(
      GL_TRIANGLES, (GLsizei)mesh.IndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
}

void Hook<Comp::Transform>::RenderHandles(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId)
{
  // Set the handles' translation to the transform's position, the rotation to
  // the frame rotation and scale depending on the distance from the camera.
  Vec3 worldTranslation = transform->GetWorldTranslation(space, ownerId);
  const Vec3& cameraTranslation = nCamera.Position();
  Math::Ray cameraRay;
  cameraRay.StartDirection(cameraTranslation, nCamera.Forward());
  Vec3 projection = cameraRay.ClosestPointTo(worldTranslation);
  Vec3 projectedDistance = projection - cameraTranslation;
  Comp::Transform& parentT = *mSpace.GetComponent<Comp::Transform>(mParent);
  parentT.SetTranslation(worldTranslation);
  parentT.SetUniformScale(Math::Magnitude(projectedDistance) * 0.3f);
  parentT.SetRotation(ReferenceFrameRotation(transform, space, ownerId));

  // Always make the outer scale ring face the camera.
  if (mMode == Mode::Scale)
  {
    Comp::Transform* xyzT = mSpace.GetComponent<Comp::Transform>(mXyz);
    Math::Quaternion xyzRot;
    Vec3 transformDirection = cameraTranslation - worldTranslation;
    xyzRot.FromTo({1.0f, 0.0f, 0.0f}, transformDirection);
    xyzRot = transform->GetWorldRotation(space, ownerId).Conjugate() * xyzRot;
    xyzT->SetRotation(xyzRot);
  }

  // Set the colors for each of the gizmo handles.
  Vec4 xColor = {0.7f, 0.0f, 0.0f, 1.0f};
  Vec4 yColor = {0.0f, 0.7f, 0.0f, 1.0f};
  Vec4 zColor = {0.0f, 0.0f, 0.7f, 1.0f};
  Vec4 xyColor = {0.7f, 0.7f, 0.0f, 1.0f};
  Vec4 xzColor = {0.7f, 0.0f, 0.7f, 1.0f};
  Vec4 yzColor = {0.0f, 0.7f, 0.7f, 1.0f};
  Vec4 xyzColor = {0.7f, 0.7f, 0.7f, 1.0f};
  Vec4 activeColor = {1.0f, 1.0f, 1.0f, 1.0f};
  switch (mOperation)
  {
  case Operation::X: xColor = activeColor; break;
  case Operation::Y: yColor = activeColor; break;
  case Operation::Z: zColor = activeColor; break;
  case Operation::Xy: xyColor = activeColor; break;
  case Operation::Xz: xzColor = activeColor; break;
  case Operation::Yz: yzColor = activeColor; break;
  case Operation::Xyz: xyzColor = activeColor; break;
  }
  if (mMode == Mode::Rotate)
  {
    xyzColor[3] *= 0.8f;
  }

  // Render the handles to a framebuffer for the renderer to use later.
  glBindFramebuffer(GL_FRAMEBUFFER, mDrawbuffer.Fbo());
  const Gfx::Shader& colorShader =
    AssLib::Get<Gfx::Shader>(AssLib::nColorShaderId);
  colorShader.SetMat4("uView", nCamera.WorldToCamera().CData());
  colorShader.SetMat4("uProj", Viewport::Perspective().CData());
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  RenderHandle(mX, xColor);
  RenderHandle(mY, yColor);
  RenderHandle(mZ, zColor);
  RenderHandle(mXyz, xyzColor);
  if (mMode != Mode::Rotate)
  {
    RenderHandle(mXy, xyColor);
    RenderHandle(mXz, xzColor);
    RenderHandle(mYz, yzColor);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  Gfx::Renderer::QueueFullscreenFramebuffer(mDrawbuffer);
}

} // namespace Editor
