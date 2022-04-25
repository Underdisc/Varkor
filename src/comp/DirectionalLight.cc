#include <imgui/imgui.h>

#include "DirectionalLight.h"

namespace Comp {

void DirectionalLight::VInit(const World::Object& owner)
{
  mDirection = smDefaultDirection;
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
}

void DirectionalLight::VSerialize(Vlk::Value& val)
{
  val("Direction") = mDirection;
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
}

void DirectionalLight::VDeserialize(const Vlk::Explorer& ex)
{
  mDirection = ex("Direction").As<Vec3>(smDefaultDirection);
  mAmbient = ex("Ambient").As<Vec3>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Vec3>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Vec3>(smDefaultSpecular);
}

} // namespace Comp

namespace Editor {

void Hook<Comp::DirectionalLight>::Edit(const World::Object& object)
{
  auto& light = object.Get<Comp::DirectionalLight>();
  ImGui::DragFloat3("Direction", light.mDirection.mD, 0.01f, -1.0f, 1.0f);
  ImGui::ColorEdit3("Ambient", light.mAmbient.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Diffuse", light.mDiffuse.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Specular", light.mSpecular.mD, ImGuiColorEditFlags_Float);
}

} // namespace Editor
