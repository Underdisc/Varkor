#include <imgui/imgui.h>

#include "Error.h"
#include "editor/ErrorInterface.h"

namespace Editor {

void ErrorInterface::Show()
{
  ImGui::Begin("Error", &mOpen);
  if (ImGui::Button("Clear", ImVec2(-1, 0)))
  {
    Error::nLog.clear();
  }
  ImGui::BeginChild(
    "Log", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
  const char* text = Error::nLog.data();
  ImGui::TextUnformatted(text, text + Error::nLog.size());
  ImGui::EndChild();
  ImGui::End();
}

} // namespace Editor
