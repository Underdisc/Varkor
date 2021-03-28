#include <imgui/imgui.h>

#include "Transform.h"

namespace Editor {
namespace Hook {

template<>
void Edit(Comp::Transform* transform)
{
  Vec3 translation = transform->GetTranslation();
  ImGui::PushItemWidth(-60.0f);
  ImGui::DragFloat3("Position", translation.mD, 0.01f);
  transform->SetTranslation(translation);

  Vec3 scale = transform->GetScale();
  ImGui::DragFloat3("Scale", scale.mD, 0.01f);
  transform->SetScale(scale);

  Quat rotation = transform->GetRotation();
  Vec3 eulerAngles = rotation.EulerAngles();
  Vec3 newAngles = eulerAngles;
  bool rotationDragged = ImGui::DragFloat3(
    "Rotation",
    newAngles.mD,
    0.01f,
    0.0f,
    0.0f,
    "%.3f",
    ImGuiSliderFlags_NoInput);
  if (rotationDragged)
  {
    Vec3 delta = newAngles - eulerAngles;
    Quat xDelta(delta[0], {1.0f, 0.0f, 0.0f});
    Quat yDelta(delta[1], {0.0f, 1.0f, 0.0f});
    Quat zDelta(delta[2], {0.0f, 0.0f, 1.0f});
    transform->SetRotation(zDelta * yDelta * xDelta * rotation);
  }
  ImGui::PopItemWidth();
}

} // namespace Hook
} // namespace Editor
