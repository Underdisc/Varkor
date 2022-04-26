#include <imgui/imgui.h>

#include "PointLight.h"
#include "editor/gizmos/Translator.h"
#include "vlk/Valkor.h"

namespace Comp {

void PointLight::VInit(const World::Object& owner)
{
  mPosition = smDefaultPosition;
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
  mConstant = smDefaultConstant;
  mLinear = smDefaultLinear;
  mQuadratic = smDefaultQuadratic;
}

void PointLight::VSerialize(Vlk::Value& val)
{
  val("Position") = mPosition;
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
  val("Constant") = mConstant;
  val("Linear") = mLinear;
  val("Quadratic") = mQuadratic;
}

void PointLight::VDeserialize(const Vlk::Explorer& ex)
{
  mPosition = ex("Position").As<Vec3>(smDefaultPosition);
  mAmbient = ex("Ambient").As<Vec3>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Vec3>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Vec3>(smDefaultSpecular);
  mConstant = ex("Constant").As<float>(smDefaultConstant);
  mLinear = ex("Linear").As<float>(smDefaultLinear);
  mQuadratic = ex("Quadratic").As<float>(smDefaultQuadratic);
}

} // namespace Comp

namespace Editor {

void Hook<Comp::PointLight>::Edit(const World::Object& object)
{
  auto& light = object.Get<Comp::PointLight>();
  Quat referenceFrame = {1.0f, 0.0f, 0.0f, 0.0f};
  light.mPosition = Gizmos::Translate(light.mPosition, referenceFrame);
  ImGui::DragFloat3("Position", light.mPosition.mD);
  ImGui::ColorEdit3("Ambient", light.mAmbient.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Diffuse", light.mDiffuse.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Specular", light.mSpecular.mD, ImGuiColorEditFlags_Float);
  ImGui::DragFloat("Constant", &light.mConstant, 0.01f, 1.0f, 2.0f);
  ImGui::DragFloat("Linear", &light.mLinear, 0.01f, 0.0f, 2.0f);
  ImGui::DragFloat("Quadratic", &light.mQuadratic, 0.01f, 0.0f, 2.0f);
}

} // namespace Editor
