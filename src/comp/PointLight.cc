#include <imgui/imgui.h>

#include "PointLight.h"

namespace Comp {

void PointLight::EditorHook()
{
  ImGui::DragFloat3("Position", mPosition.mD, 0.01f);
  ImGui::ColorEdit3("Ambient", mAmbient.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Diffuse", mDiffuse.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Specular", mSpecular.mD, ImGuiColorEditFlags_Float);
}

} // namespace Comp
