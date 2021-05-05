#include <glad/glad.h>
#include <imgui/imgui.h>

#include "Input.h"
#include "Viewport.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "editor/hook/Transform.h"
#include "gfx/Renderer.h"
#include "math/Constants.h"
#include "math/Geometry.h"
#include "math/Matrix4.h"
#include "math/Utility.h"

namespace Editor {
namespace Hook {

template<>
void Edit(Comp::Transform* transform)
{
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
}

Gizmo<Comp::Transform>::Gizmo():
  mDrawbuffer(GL_RGBA, GL_UNSIGNED_BYTE),
  mReferenceFrame(ReferenceFrame::World),
  mOperation(Operation::None)
{
  AssetLibrary::AddRequiredModel(arrowPath);
  AssetLibrary::AddRequiredModel(cubePath);
  AssetLibrary::AddRequiredModel(scalePath);
  AssetLibrary::AddRequiredModel(spherePath);
  AssetLibrary::AddRequiredModel(torusPath);

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
  xyzT.SetUniformScale(0.13f);
  xM.mAsset = arrowPath;
  yM.mAsset = arrowPath;
  zM.mAsset = arrowPath;
  xyM.mAsset = cubePath;
  xzM.mAsset = cubePath;
  yzM.mAsset = cubePath;
  xyzM.mAsset = spherePath;
}

bool Gizmo<Comp::Transform>::Run(
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
    Input::StandardMousePosition(), GetCamera().CameraToWorld());
  const Vec3& cameraPosition = GetCamera().Position();
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
  bool editing = false;
  switch (mMode)
  {
  case Mode::Translate:
    editing = Translate(transform, space, object.mMember, mouseRay);
    break;
  case Mode::Scale:
    editing = Scale(transform, space, object.mMember, mouseRay);
    break;
  case Mode::Rotate:
    editing = Rotate(transform, space, object.mMember, mouseRay);
    break;
  }
  RenderHandles(transform, space, object.mMember);
  return editing;
}

void Gizmo<Comp::Transform>::DisplayOptionsWindow()
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
  ImGui::PopItemWidth();
  ImGui::End();
}

void Gizmo<Comp::Transform>::TryStartOperation(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId,
  const Math::Ray& mouseRay)
{
  // Determine if the user began an operation.
  World::MemberId handleId =
    Gfx::Renderer::HoveredMemberId(mSpace, GetCamera().WorldToCamera());
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

bool Gizmo<Comp::Transform>::Translate(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId,
  const Math::Ray& mouseRay)
{
  Math::Ray gizmoRay;
  Math::Plane gizmoPlane;
  PrepareGizmoRepresentation(transform, space, ownerId, &gizmoRay, &gizmoPlane);
  switch (mOperation)
  {
  case Operation::X:
  case Operation::Y:
  case Operation::Z:
    if (gizmoRay.HasClosestTo(mouseRay))
    {
      Vec3 mousePoint = gizmoRay.ClosestPointTo(mouseRay);
      Vec3 worldTranslation = mousePoint - mTranslateOffset;
      transform->SetWorldTranslation(worldTranslation, space, ownerId);
    }
    return true;
  case Operation::Xy:
  case Operation::Xz:
  case Operation::Yz:
  case Operation::Xyz:
    if (Math::HasIntersection(mouseRay, gizmoPlane))
    {
      Vec3 mousePoint = Math::Intersection(mouseRay, gizmoPlane);
      Vec3 worldTranslation = mousePoint - mTranslateOffset;
      transform->SetWorldTranslation(worldTranslation, space, ownerId);
    }
    return true;
  }
  return false;
}

bool Gizmo<Comp::Transform>::Scale(
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
  const float scaleSensitivity = 0.3f;

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
    transform->SetScale(mStartScale + axis * deltaT * scaleSensitivity);
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
    Vec3 ratios;
    if (Math::Near(mStartScale, {0.0f, 0.0f, 0.0f}))
    {
      ratios = axes;
    } else
    {
      ratios = Math::Normalize(Math::ComponentwiseProduct(mStartScale, axes));
    }
    transform->SetScale(
      mStartScale +
      ratios * Math::Dot(offsetDirection, mouseDistance) * scaleSensitivity);
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

bool Gizmo<Comp::Transform>::Rotate(
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
    Vec3 up = parentRotation.Conjugate().Rotate(GetCamera().Up());
    Vec3 right = parentRotation.Conjugate().Rotate(GetCamera().Right());
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

void Gizmo<Comp::Transform>::SwitchMode(Mode newMode)
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
    xyzT->SetUniformScale(0.13f);
    xM->mAsset = arrowPath;
    yM->mAsset = arrowPath;
    zM->mAsset = arrowPath;
    xyzM->mAsset = spherePath;
    break;
  case Mode::Scale:
    xT->SetTranslation({0.5f, 0.0f, 0.0f});
    yT->SetTranslation({0.0f, 0.5f, 0.0f});
    zT->SetTranslation({0.0f, 0.0f, 0.5f});
    xyzT->SetUniformScale(1.1f);
    xM->mAsset = scalePath;
    yM->mAsset = scalePath;
    zM->mAsset = scalePath;
    xyzM->mAsset = torusPath;
    break;
  case Mode::Rotate:
    xT->SetTranslation({0.0f, 0.0f, 0.0f});
    yT->SetTranslation({0.0f, 0.0f, 0.0f});
    zT->SetTranslation({0.0f, 0.0f, 0.0f});
    xyzT->SetUniformScale(0.8f);
    xM->mAsset = torusPath;
    yM->mAsset = torusPath;
    zM->mAsset = torusPath;
    xyzM->mAsset = spherePath;
  }
}

Gizmo<Comp::Transform>::Operation Gizmo<Comp::Transform>::GetHandleOperation(
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

void Gizmo<Comp::Transform>::PrepareGizmoRepresentation(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId,
  Math::Ray* gizmoRay,
  Math::Plane* gizmoPlane) const
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
    case Operation::Xyz: gizmoPlane->Normal(GetCamera().Forward()); break;
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
}

Quat Gizmo<Comp::Transform>::ReferenceFrameRotation(
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

void Gizmo<Comp::Transform>::RenderHandle(
  World::MemberId handleId, const Vec4& color)
{
  Comp::Transform& transform = *mSpace.GetComponent<Comp::Transform>(handleId);
  Comp::Model& model = *mSpace.GetComponent<Comp::Model>(handleId);
  const Gfx::Shader& colorShader = Gfx::Renderer::ColorShader();
  colorShader.SetMat4(
    "uModel", transform.GetWorldMatrix(mSpace, handleId).CData());
  colorShader.SetVec4("uColor", color.CData());
  Gfx::Renderer::RenderModel(colorShader, model);
}

void Gizmo<Comp::Transform>::RenderHandles(
  Comp::Transform* transform,
  const World::Space& space,
  World::MemberId ownerId)
{
  // Set the handles' translation to the transform's position, the rotation to
  // the frame rotation and scale depending on the distance from the camera.
  Vec3 worldTranslation = transform->GetWorldTranslation(space, ownerId);
  const Vec3& cameraTranslation = GetCamera().Position();
  Math::Ray cameraRay;
  cameraRay.StartDirection(cameraTranslation, GetCamera().Forward());
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
  const Gfx::Shader& colorShader = Gfx::Renderer::ColorShader();
  colorShader.SetMat4("uView", GetCamera().WorldToCamera().CData());
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

} // namespace Hook
} // namespace Editor
