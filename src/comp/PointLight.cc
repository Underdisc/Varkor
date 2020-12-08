#include <imgui/imgui.h>

#include "PointLight.h"

namespace Comp {

PointLight::PointLight():
  mPosition({0.0f, 0.0f, 0.0f}),
  mAmbient({0.1f, 0.1f, 0.1f}),
  mDiffuse({0.5f, 0.5f, 0.5f}),
  mSpecular({1.0f, 1.0f, 1.0f}),
  mConstant(1.0f),
  mLinear(0.35f),
  mQuadratic(0.44f)
{}

void PointLight::EditorHook()
{
  ImGui::DragFloat3("Position", mPosition.mD, 0.01f);
  ImGui::ColorEdit3("Ambient", mAmbient.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Diffuse", mDiffuse.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Specular", mSpecular.mD, ImGuiColorEditFlags_Float);

  ImGui::DragFloat("Constant", &mConstant, 0.01f, 1.0f, 2.0f);
  ImGui::DragFloat("Linear", &mLinear, 0.01f, 0.0f, 2.0f);
  ImGui::DragFloat("Quadratic", &mQuadratic, 0.01f, 0.0f, 2.0f);
}

} // namespace Comp
