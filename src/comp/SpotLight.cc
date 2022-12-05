#include <cmath>
#include <imgui/imgui.h>

#include "SpotLight.h"
#include "editor/Utility.h"
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
  mAmbient = ex("Ambient").As<Gfx::HdrColor>(smDefaultAmbient);
  mDiffuse = ex("Diffuse").As<Gfx::HdrColor>(smDefaultDiffuse);
  mSpecular = ex("Specular").As<Gfx::HdrColor>(smDefaultSpecular);
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

void SpotLight::VEdit(const World::Object& owner)
{
  Quat referenceFrame = {1.0f, 0.0f, 0.0f, 0.0f};
  mPosition = Editor::Gizmos::Translate(mPosition, referenceFrame);
  float labelWidth = 65;
  ImGui::PushItemWidth(-labelWidth);
  ImGui::DragFloat3("Position", mPosition.mD, 0.01f);
  ImGui::DragFloat3("Direction", mDirection.mD, 0.01f, -1.0f, 1.0f);
  ImGui::DragFloat("Constant", &mConstant, 0.01f, 1.0f, 2.0f);
  ImGui::DragFloat("Linear", &mLinear, 0.01f, 0.0f, 2.0f);
  ImGui::DragFloat("Quadratic", &mQuadratic, 0.01f, 0.0f, 2.0f);
  ImGui::PopItemWidth();
  Editor::HdrColorEdit("Ambient", &mAmbient, -labelWidth);
  Editor::HdrColorEdit("Diffuse", &mDiffuse, -labelWidth);
  Editor::HdrColorEdit("Specular", &mSpecular, -labelWidth);

  ImGui::PushItemWidth(-labelWidth);
  float innerCutoff = GetInnerCutoff();
  float outerCutoff = GetOuterCutoff();
  ImGui::DragFloat("Inner Cutoff", &innerCutoff, 0.01f, 0.0f, outerCutoff);
  ImGui::DragFloat(
    "Outer Cutoff", &outerCutoff, 0.01f, innerCutoff, Math::nPiO2);
  SetInnerCutoff(innerCutoff);
  SetOuterCutoff(outerCutoff);
  ImGui::PopItemWidth();
}

} // namespace Comp
