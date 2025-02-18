#include <imgui/imgui.h>

#include "Log.h"
#include "editor/LogInterface.h"

namespace Editor {

void LogInterface::Show() {
  ImGui::Begin("Log", &mOpen);
  if (ImGui::Button("Clear", ImVec2(-1, 0))) {
    Log::nLogMutex.lock();
    Log::nLog.clear();
    Log::nLogMutex.unlock();
  }
  ImGui::BeginChild(
    "Log", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
  Log::nLogMutex.lock();
  const char* text = Log::nLog.data();
  ImGui::TextUnformatted(text, text + Log::nLog.size());
  Log::nLogMutex.unlock();
  ImGui::EndChild();
  ImGui::End();
}

} // namespace Editor
