#include <cmath>
#include <imgui/imgui.h>

#include "SpotLight.h"
#include "editor/gizmos/Translator.h"
#include "math/Constants.h"
#include "vlk/Valkor.h"

namespace Comp {

void SpotLight::VInit(const World::Object& owner)
{
  mPosition = smDefaultPosition;
  mDirection = smDefaultDirection;
  mAmbient = smDefaultAmbient;
  mDiffuse = smDefaultDiffuse;
  mSpecular = smDefaultSpecular;
  mConstant = smDefaultConstant;
  mLinear = smDefaultLinear;
  mQuadratic = smDefaultQuadratic;
  SetInnerCutoff(smDefaultInnerCutoff);
  SetOuterCutoff(smDefaultOuterCutoff);
}

void SpotLight::VSerialize(Vlk::Value& val)
{
  val("Position") = mPosition;
  val("Direction") = mDirection;
  val("Ambient") = mAmbient;
  val("Diffuse") = mDiffuse;
  val("Specular") = mSpecular;
  val("Constant") = mConstant;
  val("Linear") = mLinear;
  val("Quadratic") = mQuadratic;
  val("InnerCutoff") = GetInnerCutoff();
  val("OuterCutoff") = GetOuterCutoff();
}

void SpotLight::VDeserialize(const Vlk::Explorer& ex)
{
  mPosition = ex("Position").As<Vec3>(smDefaultPosition);
  mDirection = ex("Direction").As<Vec3>(smDefaultDirection);
  mAmbient = ex("Ambient").As<Vec3>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Vec3>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Vec3>(smDefaultSpecular);
  mConstant = ex("Constant").As<float>(smDefaultConstant);
  mLinear = ex("Linear").As<float>(smDefaultLinear);
  mQuadratic = ex("Quadratic").As<float>(smDefaultQuadratic);
  SetInnerCutoff(ex("InnerCutoff").As<float>(smDefaultInnerCutoff));
  SetOuterCutoff(ex("OuterCutoff").As<float>(smDefaultOuterCutoff));
}

void SpotLight::SetInnerCutoff(float angle)
{
  mInnerCutoff = std::cosf(angle);
}

void SpotLight::SetOuterCutoff(float angle)
{
  mOuterCutoff = std::cosf(angle);
}

float SpotLight::GetInnerCutoff() const
{
  return std::acosf(mInnerCutoff);
}

float SpotLight::GetOuterCutoff() const
{
  return std::acosf(mOuterCutoff);
}

} // namespace Comp

namespace Editor {

void Hook<Comp::SpotLight>::Edit(const World::Object& object)
{
  auto& light = object.Get<Comp::SpotLight>();
  Quat referenceFrame = {1.0f, 0.0f, 0.0f, 0.0f};
  light.mPosition = Gizmos::Translate(light.mPosition, referenceFrame);
  ImGui::DragFloat3("Position", light.mPosition.mD, 0.01f);
  ImGui::DragFloat3("Direction", light.mDirection.mD, 0.01f, -1.0f, 1.0f);
  ImGui::ColorEdit3("Ambient", light.mAmbient.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Diffuse", light.mDiffuse.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Specular", light.mSpecular.mD, ImGuiColorEditFlags_Float);
  ImGui::DragFloat("Constant", &light.mConstant, 0.01f, 1.0f, 2.0f);
  ImGui::DragFloat("Linear", &light.mLinear, 0.01f, 0.0f, 2.0f);
  ImGui::DragFloat("Quadratic", &light.mQuadratic, 0.01f, 0.0f, 2.0f);

  float innerCutoff = light.GetInnerCutoff();
  float outerCutoff = light.GetOuterCutoff();
  ImGui::DragFloat("Inner Cutoff", &innerCutoff, 0.01f, 0.0f, outerCutoff);
  ImGui::DragFloat(
    "Outer Cutoff", &outerCutoff, 0.01f, innerCutoff, Math::nPiO2);
  light.SetInnerCutoff(innerCutoff);
  light.SetOuterCutoff(outerCutoff);
}

} // namespace Editor