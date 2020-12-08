#include <cmath>
#include <imgui/imgui.h>

#include "SpotLight.h"
#include "math/Constants.h"

namespace Comp {

SpotLight::SpotLight():
  mPosition({0.0f, 0.0f, 0.0f}),
  mDirection({0.0f, -1.0f, 0.0f}),
  mAmbient({0.1f, 0.1f, 0.1f}),
  mDiffuse({0.5f, 0.5f, 0.5f}),
  mSpecular({1.0f, 1.0f, 1.0f}),
  mConstant(1.0f),
  mLinear(0.35f),
  mQuadratic(0.44f)
{
  SetInnerCutoff(PIf / 6.0f);
  SetOuterCutoff(PIf / 5.0f);
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

void SpotLight::EditorHook()
{
  ImGui::DragFloat3("Position", mPosition.mD, 0.01f);
  ImGui::DragFloat3("Direction", mDirection.mD, 0.01f, -1.0f, 1.0f);

  ImGui::ColorEdit3("Ambient", mAmbient.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Diffuse", mDiffuse.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Specular", mSpecular.mD, ImGuiColorEditFlags_Float);

  ImGui::DragFloat("Constant", &mConstant, 0.01f, 1.0f, 2.0f);
  ImGui::DragFloat("Linear", &mLinear, 0.01f, 0.0f, 2.0f);
  ImGui::DragFloat("Quadratic", &mQuadratic, 0.01f, 0.0f, 2.0f);

  float innerCutoff = GetInnerCutoff();
  float outerCutoff = GetOuterCutoff();
  ImGui::DragFloat("Inner Cutoff", &innerCutoff, 0.01f, 0.0f, outerCutoff);
  ImGui::DragFloat("Outer Cutoff", &outerCutoff, 0.01f, innerCutoff, PIO2f);
  SetInnerCutoff(innerCutoff);
  SetOuterCutoff(outerCutoff);
}

} // namespace Comp
