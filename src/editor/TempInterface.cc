#include <imgui/imgui.h>

#include "editor/TempInterface.h"

namespace Editor {

void TempInterface::Show()
{
  ImGui::Begin("Temp", &mOpen);
  ImGui::End();
}

} // namespace Editor