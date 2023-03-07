#include "editor/Camera.h"
#include "Input.h"
#include "Temporal.h"
#include "editor/Editor.h"
#include "math/Constants.h"
#include "world/Object.h"

namespace Editor {

void Camera::Init()
{
  mCameraId = Editor::nSpace.CreateMember();
  GetObject().Add<Comp::Camera>();

  mYaw = 0.0f;
  mPitch = 0.0f;

  mSpeed = 1.0f;
  mSensitivity = 0.001f * Math::nPi;
}

void Camera::Purge()
{
  GetObject().Delete();
}

World::Object Camera::GetObject()
{
  return World::Object(&Editor::nSpace, mCameraId);
}

void Camera::Update()
{
  // LeftControl disables camera motion because it's the editor's modifier key.
  if (Input::KeyDown(Input::Key::LeftControl)) {
    return;
  }

  // Change the camera's yaw and pitch depending on input.
  const World::Object cameraObject = GetObject();
  auto& transformComp = cameraObject.Get<Comp::Transform>();
  auto& cameraComp = cameraObject.Get<Comp::Camera>();
  if (Input::MouseDown(Input::Mouse::Right)) {
    Vec2 mouseMotion = Input::MouseMotion();
    mYaw -= mouseMotion[0] * mSensitivity;
    mPitch -= mouseMotion[1] * mSensitivity;
    Math::Quaternion hRot, vRot;
    hRot.AngleAxis(mYaw, {0.0f, 1.0f, 0.0f});
    vRot.AngleAxis(mPitch, {1.0f, 0.0f, 0.0f});
    Math::Quaternion rotation = hRot * vRot;
    transformComp.SetRotation(rotation);
  }

  // Change the camera speed using scroll wheel input.
  const Vec2& scroll = Input::MouseScroll();
  constexpr float scrollSensitivity = 0.1f;
  mSpeed = mSpeed + scroll[1] * scrollSensitivity * mSpeed;
  constexpr float minCameraSpeed = 0.1f;
  if (mSpeed < minCameraSpeed) {
    mSpeed = minCameraSpeed;
  }

  // Change the camera position depending on input.
  Vec3 translation = transformComp.GetTranslation();
  Vec3 forward = cameraComp.WorldForward(cameraObject);
  Vec3 right = cameraComp.WorldRight(cameraObject);
  Vec3 up = cameraComp.WorldUp(cameraObject);
  if (Input::KeyDown(Input::Key::W)) {
    translation += forward * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::S)) {
    translation -= forward * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::D)) {
    translation += right * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::A)) {
    translation -= right * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::E)) {
    translation += up * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::KeyDown(Input::Key::Q)) {
    translation -= up * Temporal::DeltaTime() * mSpeed;
  }
  transformComp.SetTranslation(translation);
}

} // namespace Editor
