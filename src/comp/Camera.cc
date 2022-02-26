#include "comp/Camera.h"
#include "Viewport.h"
#include "comp/Transform.h"
#include "math/Constants.h"

namespace Comp {

float Camera::smDefaultFov = Math::nPi / 2.0f;
float Camera::smDefaultNear = 0.1f;
float Camera::smDefaultFar = 1000000.0f;

void Camera::VInit()
{
  mFov = smDefaultFov;
  mNear = smDefaultNear;
  mFar = smDefaultFar;
}

void Camera::VSerialize(Vlk::Value& cameraVal)
{
  cameraVal("Fov") = mFov;
  cameraVal("Near") = mNear;
  cameraVal("Far") = mFar;
}

void Camera::VDeserialize(const Vlk::Explorer& cameraEx)
{
  mFov = cameraEx("Fov").As<float>(smDefaultFov);
  mNear = cameraEx("Near").As<float>(smDefaultNear);
  mFar = cameraEx("Far").As<float>(smDefaultFar);
}

// This will make the camera look at a position in its local space.
void Camera::LocalLookAt(
  const Vec3& localPosition, const Vec3& localUp, const World::Object& owner)
{
  Transform& transform = *owner.GetComponent<Transform>();
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
  if (!parent.Valid())
  {
    LocalLookAt(worldPosition, worldUp, owner);
    return;
  }
  Transform* parentTransform = parent.GetComponent<Transform>();
  if (parentTransform == nullptr)
  {
    LocalLookAt(worldPosition, worldUp, owner);
    return;
  }
  Transform& transform = *owner.GetComponent<Transform>();
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
  Mat4 proj;
  Math::Perspective(&proj, mFov, Viewport::Aspect(), mNear, mFar);
  return proj;
}

// Standard position represents a location on the monitor using values in the
// range of [0, 1].
Vec3 Camera::StandardToWorldPosition(
  Vec2 standardPosition, const Mat4& inverseView) const
{
  // The value returned by this function will be on the camera frustrum's near
  // plane.
  float heightOver2 = std::tanf(mFov / 2.0f) * mNear;
  standardPosition[0] *= heightOver2 * Viewport::Aspect();
  standardPosition[1] *= heightOver2;
  Vec4 worldPosition = {standardPosition[0], standardPosition[1], -mNear, 1.0f};
  worldPosition = inverseView * worldPosition;
  return (Vec3)worldPosition;
}

} // namespace Comp

namespace Editor {

bool Hook<Comp::Camera>::Edit(const World::Object& object)
{
  Comp::Camera* cameraComp = object.GetComponent<Comp::Camera>();
  ImGui::DragFloat("FoV", &cameraComp->mFov, 0.001f, 0.1f, Math::nPi - 0.1f);
  return false;
}

} // namespace Editor
