#include <imgui/imgui.h>
#include <string>

#include "Viewport.h"
#include "comp/Camera.h"
#include "comp/Transform.h"
#include "math/Constants.h"

namespace Comp {

const char* Camera::smProjectionTypeNames[2] = {"Perspective", "Orthographic"};
float Camera::smDefaultFov = Math::nPi / 2.0f;
float Camera::smDefaultHeight = 10.0f;
float Camera::smMinimumPerspectiveNear = 0.1f;
float Camera::smMinimumOrthographicNear = 0.0f;
float Camera::smMaximumFar = 1000000.0f;

void Camera::VInit(const World::Object& owner)
{
  mProjectionType = ProjectionType::Perspective;
  mFov = smDefaultFov;
  mNear = smMinimumPerspectiveNear;
  mFar = smMaximumFar;
}

void Camera::VSerialize(Vlk::Value& cameraVal)
{
  Vlk::Value& projectionTypeVal = cameraVal("ProjectionType");
  switch (mProjectionType) {
  case ProjectionType::Perspective:
    projectionTypeVal = smProjectionTypeNames[0];
    cameraVal("Fov") = mFov;
    break;
  case ProjectionType::Orthographic:
    projectionTypeVal = smProjectionTypeNames[1];
    cameraVal("Height") = mHeight;
    break;
  }
  cameraVal("Near") = mNear;
  cameraVal("Far") = mFar;
}

void Camera::VDeserialize(const Vlk::Explorer& cameraEx)
{
  std::string projectionTypeName =
    cameraEx("ProjectionType").As<std::string>(smProjectionTypeNames[0]);
  if (projectionTypeName == smProjectionTypeNames[1]) {
    mProjectionType = ProjectionType::Orthographic;
    mHeight = cameraEx("Height").As<float>(smDefaultHeight);
    mNear = cameraEx("Near").As<float>(smMinimumOrthographicNear);
  }
  else {
    mProjectionType = ProjectionType::Perspective;
    mFov = cameraEx("Fov").As<float>(smDefaultFov);
    mNear = cameraEx("Near").As<float>(smMinimumPerspectiveNear);
  }
  mFar = cameraEx("Far").As<float>(smMaximumFar);
}

void Camera::VRenderable(const World::Object& owner)
{
  Gfx::Renderable::Icon icon;
  icon.mOwner = owner.mMemberId;
  auto& transform = owner.Get<Transform>();
  icon.mTranslation = transform.GetWorldTranslation(owner);
  icon.mColor = {0.0f, 1.0f, 0.0f, 1.0f};
  icon.mMeshId = "vres/renderer:CameraIcon";
  Gfx::Collection::Add(std::move(icon));
}

// This will make the camera look at a position in its local space.
void Camera::LocalLookAt(
  const Vec3& localPosition, const Vec3& localUp, const World::Object& owner)
{
  Transform& transform = owner.GetComponent<Transform>();
  LogAbortIf(
    Math::Near(localPosition, transform.GetTranslation()),
    "The Camera cannot look at its current position.");
  Vec3 direction = localPosition - transform.GetTranslation();
  Vec3 back = Math::Normalize(-direction);
  Vec3 right = Math::Normalize(Math::Cross(localUp, back));
  Vec3 up = Math::Cross(back, right);

  Math::Quaternion newRotation;
  newRotation.BasisVectors(right, up, back);
  transform.SetRotation(newRotation);
}

// This will make the camera look at a position in world space. If the camera
// has a parent lineage, this will account for the parent transformations.
void Camera::WorldLookAt(
  const Vec3& worldPosition, const Vec3& worldUp, const World::Object& owner)
{
  World::Object parent = owner.Parent();
  if (!parent.Valid()) {
    LocalLookAt(worldPosition, worldUp, owner);
    return;
  }
  Transform* parentTransform = parent.TryGetComponent<Transform>();
  if (parentTransform == nullptr) {
    LocalLookAt(worldPosition, worldUp, owner);
    return;
  }
  Transform& transform = owner.GetComponent<Transform>();
  Vec3 localPosition = transform.WorldToLocalTranslation(worldPosition, owner);
  Quaternion parentWorldRotation = parentTransform->GetWorldRotation(parent);
  // Consider worldUp's direction if it were in local space. If we undo the
  // rotation by applying the conjugate, we get a vector in local space that
  // points in the same direction as worldUp in world space.
  Vec3 localUp = parentWorldRotation.Conjugate().Rotate(worldUp);
  LocalLookAt(localPosition, localUp, owner);
}

Mat4 Camera::Proj() const
{
  Mat4 projection;
  switch (mProjectionType) {
  case ProjectionType::Perspective:
    Math::Perspective(&projection, mFov, Viewport::Aspect(), mNear, mFar);
    break;
  case ProjectionType::Orthographic:
    Math::Orthographic(&projection, mHeight, Viewport::Aspect(), mNear, mFar);
    break;
  }
  return projection;
}

Mat4 Camera::View(const World::Object& owner) const
{
  auto& transformComp = owner.Get<Transform>();
  Quat worldRotation = transformComp.GetWorldRotation(owner);
  Vec3 worldTranslation = transformComp.GetWorldTranslation(owner);
  Mat4 rotate, translate;
  Math::Rotate(&rotate, worldRotation.Conjugate());
  Math::Translate(&translate, -worldTranslation);
  return rotate * translate;
}

Mat4 Camera::InverseView(const World::Object& owner) const
{
  auto& transformComp = owner.Get<Transform>();
  Quat worldRotation = transformComp.GetWorldRotation(owner);
  Vec3 worldTranslation = transformComp.GetWorldTranslation(owner);
  Mat4 translate, rotate;
  Math::Translate(&translate, worldTranslation);
  Math::Rotate(&rotate, worldRotation);
  return translate * rotate;
}

Vec3 Camera::WorldTranslation(const World::Object& owner) const
{
  auto& transformComp = owner.Get<Comp::Transform>();
  return transformComp.GetWorldTranslation(owner);
}

Quat Camera::WorldRotation(const World::Object& owner) const
{
  auto& transformComp = owner.Get<Comp::Transform>();
  return transformComp.GetWorldRotation(owner);
}

Vec3 Camera::WorldForward(const World::Object& owner) const
{
  return WorldRotation(owner).Rotate({0.0f, 0.0f, -1.0f});
}

Vec3 Camera::WorldRight(const World::Object& owner) const
{
  return WorldRotation(owner).Rotate({1.0f, 0.0f, 0.0f});
}

Vec3 Camera::WorldUp(const World::Object& owner) const
{
  return WorldRotation(owner).Rotate({0.0f, 1.0f, 0.0f});
}

float Camera::ProjectedDistance(
  const World::Object& owner, const Vec3& worldTranslation) const
{
  Math::Ray viewRay;
  Vec3 viewPos = WorldTranslation(owner);
  viewRay.StartDirection(viewPos, WorldForward(owner));
  Vec3 projection = viewRay.ClosestPointTo(worldTranslation);
  Vec3 distance = projection - viewPos;
  if (Math::Near(distance, {0.0f, 0.0f, 0.0f})) {
    return 0.0f;
  }
  return Math::Magnitude(distance);
}

// Retruns a point's ndc.
Vec2 Camera::WorldTranslationToStandardTranslation(
  Vec3 worldTranslation, const World::Object& owner) const
{
  Vec4 projectedPoint = (Vec4)worldTranslation;
  projectedPoint[3] = 1;
  projectedPoint = Proj() * View(owner) * projectedPoint;
  projectedPoint /= projectedPoint[3];
  return (Vec2)projectedPoint;
}

// This will take a position in the window and convert it to a position in the
// world. The values in standard position should be in the range of [-1, 1],
// where -1 represents the leftmost and bottommost edges of the window. The
// value returned will be on the view frustrum's near plane.
Vec3 Camera::StandardTranslationToWorldTranslation(
  Vec2 standardPosition, const World::Object& owner) const
{
  float heightOver2;
  switch (mProjectionType) {
  case ProjectionType::Perspective:
    heightOver2 = tanf(mFov / 2.0f) * mNear;
    break;
  case ProjectionType::Orthographic: heightOver2 = mHeight / 2.0f; break;
  }
  standardPosition[0] *= heightOver2 * Viewport::Aspect();
  standardPosition[1] *= heightOver2;
  Vec4 worldPosition = {standardPosition[0], standardPosition[1], -mNear, 1.0f};
  worldPosition = InverseView(owner) * worldPosition;
  return (Vec3)worldPosition;
}

// Imagine a position on the window extending as a line into space such that we
// are only able to see it as a single point. This function will return a
// ray that represents exactly that.
Math::Ray Camera::StandardTranslationToWorldRay(
  const Vec2& standardPosition, const World::Object& owner) const
{
  Math::Ray ray;
  Vec3 cameraTranslation = WorldTranslation(owner);
  Vec3 worldTranslation =
    StandardTranslationToWorldTranslation(standardPosition, owner);
  switch (mProjectionType) {
  case ProjectionType::Perspective:
    ray.StartDirection(cameraTranslation, worldTranslation - cameraTranslation);
    break;
  case ProjectionType::Orthographic:
    ray.StartDirection(worldTranslation, WorldForward(owner));
    break;
  }
  return ray;
}

void Camera::VEdit(const World::Object& owner)
{
  const int projectionNameCount =
    sizeof(Comp::Camera::smProjectionTypeNames) / sizeof(const char*);
  int intProj = (int)mProjectionType;
  ImGui::Combo(
    "Projection Type",
    &intProj,
    Comp::Camera::smProjectionTypeNames,
    projectionNameCount);
  Comp::Camera::ProjectionType newType = (Comp::Camera::ProjectionType)intProj;
  if (newType != mProjectionType) {
    mProjectionType = newType;
    switch (newType) {
    case Comp::Camera::ProjectionType::Perspective:
      mFov = Comp::Camera::smDefaultFov;
      mNear = Comp::Camera::smMinimumPerspectiveNear;
      break;
    case Comp::Camera::ProjectionType::Orthographic:
      mHeight = Comp::Camera::smDefaultHeight;
      mNear = Comp::Camera::smMinimumOrthographicNear;
      break;
    }
  }
  switch (mProjectionType) {
  case Comp::Camera::ProjectionType::Perspective:
    ImGui::DragFloat("FoV", &mFov, 0.001f, 0.1f, Math::nPi - 0.1f);
    ImGui::DragFloat(
      "Near",
      &mNear,
      0.1f,
      Comp::Camera::smMinimumPerspectiveNear,
      mFar - Math::nEpsilonL);
    break;
  case Comp::Camera::ProjectionType::Orthographic:
    ImGui::DragFloat("Height", &mHeight, 0.01f, 0.1f, 1000.0f);
    ImGui::DragFloat(
      "Near",
      &mNear,
      0.1f,
      Comp::Camera::smMinimumOrthographicNear,
      mFar - Math::nEpsilonL);
    break;
  }
  ImGui::DragFloat(
    "Far", &mFar, 0.1f, mNear + Math::nEpsilonL, Comp::Camera::smMaximumFar);
}

} // namespace Comp
