#include <imgui/imgui.h>

#include "CameraOrbiter.h"
#include "Temporal.h"
#include "comp/Camera.h"
#include "comp/Transform.h"
#include "math/Constants.h"

namespace Comp {

void CameraOrbiter::VInit(const World::Object& owner)
{
  mPosition = {0.0f, 0.0f, 0.0f};
  mDistance = 10.0f;
  mHeight = 10.0f;
  mRate = 0.2f;
}

void CameraOrbiter::VSerialize(Vlk::Value& val)
{
  val("Position") = mPosition;
  val("Distance") = mDistance;
  val("Height") = mHeight;
  val("Rate") = mRate;
}

void CameraOrbiter::VDeserialize(const Vlk::Explorer& ex)
{
  mPosition = ex("Position").As<Vec3>({0.0f, 0.0f, 0.0f});
  mDistance = ex("Distance").As<float>(10.0f);
  mHeight = ex("Height").As<float>(10.0f);
  mRate = ex("Rate").As<float>(0.2f);
}

void CameraOrbiter::VUpdate(const World::Object& owner)
{
  Comp::Transform& transform = owner.Get<Comp::Transform>();
  Comp::Camera& camera = owner.Get<Comp::Camera>();
  float radians = Temporal::TotalTime() * mRate * Math::nPi;
  float yTheta = radians;
  Vec3 translation = {cosf(yTheta), 0.0f, sinf(yTheta)};
  translation *= mDistance;
  translation[1] = mHeight;
  transform.SetTranslation(translation);
  camera.LocalLookAt(mPosition, {0.0f, 1.0f, 0.0f}, owner);
}

void CameraOrbiter::VEdit(const World::Object& owner)
{
  ImGui::DragFloat3("Position", mPosition.mD, 0.001f);
  ImGui::DragFloat("Distance", &mDistance, 0.01f);
  ImGui::DragFloat("Height", &mHeight, 0.01f, 0.0f);
  ImGui::DragFloat("Rate", &mRate, 0.001f);
}

} // namespace Comp
