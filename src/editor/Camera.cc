#include "Input.h"
#include "Temporal.h"
#include "math/Constants.h"

#include "Camera.h"

namespace Editor {

Camera::Camera()
{
  mTransform.VInit();
  mCamera.VInit();
  mYaw = 0.0f;
  mPitch = 0.0f;

  mSpeed = 1.0f;
  mSensitivity = 0.001f * Math::nPi;
}

void Camera::Update()
{
  // LeftControl disables camera motion because it's the editor's modifier key.
  if (Input::KeyDown(Input::Key::LeftControl))
  {
    return;
  }

  // Change the camera's yaw and pitch depending on input.
  if (Input::MouseDown(Input::Mouse::Right))
  {
    Vec2 mouseMotion = Input::MouseMotion();
    mYaw -= mouseMotion[0] * mSensitivity;
    mPitch -= mouseMotion[1] * mSensitivity;
    Math::Quaternion hRot, vRot;
    hRot.AngleAxis(mYaw, {0.0f, 1.0f, 0.0f});
    vRot.AngleAxis(mPitch, {1.0f, 0.0f, 0.0f});
    Math::Quaternion rotation = hRot * vRot;
    mTransform.SetRotation(rotation);
  }

  // Change the camera speed using scroll wheel input.
  const Vec2& scroll = Input::MouseScroll();
  constexpr float scrollSensitivity = 0.1f;
  mSpeed = mSpeed + scroll[1] * scrollSensitivity * mSpeed;
  constexpr float minCameraSpeed = 0.1f;
  if (mSpeed < minCameraSpeed)
  {
    mSpeed = minCameraSpeed;
  }

  // Change the camera position depending on input.
  Vec3 translation = mTransform.GetTranslation();
  if (Input::KeyDown(Input::Key::W))
  {
    translation += Forward() * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::S))
  {
    translation -= Forward() * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::D))
  {
    translation += Right() * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::A))
  {
    translation -= Right() * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::E))
  {
    translation += Up() * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::Q))
  {
    translation -= Up() * Temporal::DeltaTime() * mSpeed;
  }
  mTransform.SetTranslation(translation);
}

Mat4 Camera::View() const
{
  return mTransform.GetInverseLocalMatrix();
}

const Mat4& Camera::InverseView()
{
  return mTransform.GetLocalMatrix();
}

Mat4 Camera::Proj() const
{
  return mCamera.Proj();
}

Vec3 Camera::Forward() const
{
  return mTransform.GetRotation().Rotate({0.0f, 0.0f, -1.0f});
}

Vec3 Camera::Right() const
{
  return mTransform.GetRotation().Rotate({1.0f, 0.0f, 0.0f});
}

Vec3 Camera::Up() const
{
  return mTransform.GetRotation().Rotate({0.0f, 1.0f, 0.0f});
}

const Vec3& Camera::Position() const
{
  return mTransform.GetTranslation();
}

// Imagine a position on the window extending as a line into space such that we
// are only able to see it as a single point. This function will return a
// ray that represents exactly that.
Math::Ray Camera::StandardPositionToRay(const Vec2& standardPosition)
{
  Vec3 worldPosition =
    mCamera.StandardToWorldPosition(standardPosition, InverseView());
  Math::Ray ray;
  switch (mCamera.mProjectionType)
  {
  case Comp::Camera::ProjectionType::Perspective:
    ray.StartDirection(Position(), worldPosition - Position());
    break;
  case Comp::Camera::ProjectionType::Orthographic:
    ray.StartDirection(worldPosition, Forward());
    break;
  }
  return ray;
}

} // namespace Editor
