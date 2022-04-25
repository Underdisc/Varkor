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

// This will take a position in the window and convert it to a position in the
// world. The values in standard position should be in the range of [-1, 1],
// where -1 represents the leftmost and bottommost edges of the window. The
// value returned will be on the view frustrum's near plane.
Vec3 Camera::StandardToWorldPosition(
  Vec2 standardPosition, const Mat4& inverseView) const
{
  float heightOver2;
  switch (mProjectionType) {
  case ProjectionType::Perspective:
    heightOver2 = std::tanf(mFov / 2.0f) * mNear;
    break;
  case ProjectionType::Orthographic: heightOver2 = mHeight / 2.0f; break;
  }
  standardPosition[0] *= heightOver2 * Viewport::Aspect();
  standardPosition[1] *= heightOver2;
  Vec4 worldPosition = {standardPosition[0], standardPosition[1], -mNear, 1.0f};
  worldPosition = inverseView * worldPosition;
  return (Vec3)worldPosition;
}

} // namespace Comp

namespace Editor {

void Hook<Comp::Camera>::Edit(const World::Object& object)
{
  Comp::Camera& cameraComp = object.GetComponent<Comp::Camera>();
  const int projectionNameCount =
    sizeof(Comp::Camera::smProjectionTypeNames) / sizeof(const char*);
  int intProj = (int)cameraComp.mProjectionType;
  ImGui::Combo(
    "Projection Type",
    &intProj,
    Comp::Camera::smProjectionTypeNames,
    projectionNameCount);
  Comp::Camera::ProjectionType newType = (Comp::Camera::ProjectionType)intProj;
  if (newType != cameraComp.mProjectionType) {
    cameraComp.mProjectionType = newType;
    switch (newType) {
    case Comp::Camera::ProjectionType::Perspective:
      cameraComp.mFov = Comp::Camera::smDefaultFov;
      cameraComp.mNear = Comp::Camera::smMinimumPerspectiveNear;
      break;
    case Comp::Camera::ProjectionType::Orthographic:
      cameraComp.mHeight = Comp::Camera::smDefaultHeight;
      cameraComp.mNear = Comp::Camera::smMinimumOrthographicNear;
      break;
    }
  }
  switch (cameraComp.mProjectionType) {
  case Comp::Camera::ProjectionType::Perspective:
    ImGui::DragFloat("FoV", &cameraComp.mFov, 0.001f, 0.1f, Math::nPi - 0.1f);
    ImGui::DragFloat(
      "Near",
      &cameraComp.mNear,
      0.1f,
      Comp::Camera::smMinimumPerspectiveNear,
      cameraComp.mFar);
    break;
  case Comp::Camera::ProjectionType::Orthographic:
    ImGui::DragFloat("Height", &cameraComp.mHeight, 0.01f, 0.1f, 1000.0f);
    ImGui::DragFloat(
      "Near",
      &cameraComp.mNear,
      0.1f,
      Comp::Camera::smMinimumOrthographicNear,
      cameraComp.mFar);
    break;
  }
  ImGui::DragFloat(
    "Far",
    &cameraComp.mFar,
    0.1f,
    cameraComp.mNear,
    Comp::Camera::smMaximumFar);
}

} // namespace Editor
