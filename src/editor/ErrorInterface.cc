#include <imgui/imgui.h>

#include "Error.h"
#include "editor/ErrorInterface.h"

namespace Editor {

void ErrorInterface::Show()
{
  ImGui::Begin("Error", &mOpen);
  if (ImGui::Button("Clear", ImVec2(-1, 0)))
  {
    Error::nLogMutex.lock();
    Error::nLog.clear();
    Error::nLogMutex.unlock();
  }
  ImGui::BeginChild(
    "Log", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
  Error::nLogMutex.lock();
  const char* text = Error::nLog.data();
  ImGui::TextUnformatted(text, text + Error::nLog.size());
  Error::nLogMutex.unlock();
  ImGui::EndChild();
  ImGui::End();
}

} // namespace Editor
