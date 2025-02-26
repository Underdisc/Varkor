#include <imgui/imgui.h>

#include "Input.h"
#include "Temporal.h"
#include "editor/Camera.h"
#include "editor/Editor.h"
#include "editor/Utility.h"
#include "math/Constants.h"
#include "world/Object.h"

namespace Editor {

Camera nCamera;

void CameraInterface::Show() {
  ImGui::Begin("Camera");
  ImGui::PushItemWidth(-Editor::CalcBufferWidth("TranslationT"));
  ImGui::DragFloat(
    "Speed", &nCamera.mSpeed, 1, 0, 0, "%.5f", ImGuiSliderFlags_Logarithmic);
  ImGui::SliderFloat("TranslationT", &nCamera.mTranslationT, 0, 1);
  ImGui::SliderFloat("Sensitivity", &nCamera.mSensitivity, 0, 0.02f, "%.5f");
  ImGui::SliderFloat("RotationT", &nCamera.mRotationT, 0, 1);
  ImGui::PopItemWidth();
  ImGui::End();
}

void Camera::Init() {
  mSpeed = 1.0f;
  mTranslationT = 0.5f;
  mSensitivity = 0.001f * Math::nPi;
  mRotationT = 0.5f;

  mCameraId = Editor::nSpace.CreateMember();
  GetObject().Add<Comp::Camera>();
  mTargetTranslation = {0, 0, 0};
  mEulerRotation = {0, 0};
  mTargetEulerRotation = {0, 0};
}

void Camera::Purge() {
  GetObject().Delete();
}

World::Object Camera::GetObject() {
  return World::Object(&Editor::nSpace, mCameraId);
}

void Camera::Update() {
  // Change the camera's yaw and pitch depending on input.
  const World::Object cameraObject = GetObject();
  auto& transformComp = cameraObject.Get<Comp::Transform>();
  auto& cameraComp = cameraObject.Get<Comp::Camera>();
  if (Input::MouseDown(Input::Mouse::Right)) {
    Vec2 mouseMotion = Input::MouseMotion();
    mTargetEulerRotation -= mouseMotion * mSensitivity;
  }
  mEulerRotation += mRotationT * (mTargetEulerRotation - mEulerRotation);
  Quat hRot = Quat::AngleAxis(mEulerRotation[0], {0.0f, 1.0f, 0.0f});
  Quat vRot = Quat::AngleAxis(mEulerRotation[1], {1.0f, 0.0f, 0.0f});
  Math::Quaternion rotation = hRot * vRot;
  transformComp.SetRotation(rotation);

  // Change the camera speed using scroll wheel input.
  const Vec2& scroll = Input::MouseScroll();
  constexpr float scrollSensitivity = 0.1f;
  mSpeed = mSpeed + scroll[1] * scrollSensitivity * mSpeed;
  constexpr float minCameraSpeed = 0.1f;
  if (mSpeed < minCameraSpeed) {
    mSpeed = minCameraSpeed;
  }

  // Change the camera position depending on input.
  Vec3 forward = cameraComp.WorldForward(cameraObject);
  Vec3 right = cameraComp.WorldRight(cameraObject);
  Vec3 up = cameraComp.WorldUp(cameraObject);
  if (Input::ActionActive(Input::Action::MoveForward)) {
    mTargetTranslation += forward * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::ActionActive(Input::Action::MoveBackward)) {
    mTargetTranslation -= forward * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::ActionActive(Input::Action::MoveRight)) {
    mTargetTranslation += right * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::ActionActive(Input::Action::MoveLeft)) {
    mTargetTranslation -= right * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::ActionActive(Input::Action::MoveUp)) {
    mTargetTranslation += up * Temporal::DeltaTime() * mSpeed;
  }
  if (Input::ActionActive(Input::Action::MoveDown)) {
    mTargetTranslation -= up * Temporal::DeltaTime() * mSpeed;
  }
  Vec3 translation = transformComp.GetTranslation();
  translation += mTranslationT * (mTargetTranslation - translation);
  transformComp.SetTranslation(translation);
}

} // namespace Editor
