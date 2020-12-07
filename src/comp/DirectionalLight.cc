#include <imgui/imgui.h>

#include "DirectionalLight.h"

namespace Comp {

void DirectionalLight::EditorHook()
{
  ImGui::DragFloat3("Direction", mDirection.mD, 0.01f, -1.0f, 1.0f);
  ImGui::ColorEdit3("Ambient", mAmbient.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Diffuse", mDiffuse.mD, ImGuiColorEditFlags_Float);
  ImGui::ColorEdit3("Specular", mSpecular.mD, ImGuiColorEditFlags_Float);
}

} // namespace Comp
