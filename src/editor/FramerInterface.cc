#include <imgui/imgui.h>

#include "Framer.h"
#include "editor/FramerInterface.h"

namespace Editor {

void FramerInterface::Show() {
  ImGui::Begin("Framer", &mOpen, ImGuiWindowFlags_AlwaysAutoResize);

  // Display the statistics.
  ImGui::Text("FPS: %f", Framer::GetAverageFps());
  ImGui::Text("Frame Usage");
  ImGui::SameLine();
  ImGui::ProgressBar(Framer::GetAverageFrameUsage(), ImVec2(0.0f, 0.0f));
  ImGui::SliderFloat(
    "Update Period", &Framer::nAverageUpdatePeriod, 0.01f, 1.0f);

  // Display the option for vsync.
  ImGui::Separator();
  bool vSync = Framer::VSyncEnabled();
  ImGui::Checkbox("VSync", &vSync);
  if (vSync != Framer::VSyncEnabled()) {
    Framer::SetVSync(vSync);
  }
  if (vSync == true) {
    ImGui::End();
    return;
  }

  // Display the option for uncapping the framerate.
  bool uncappedStatus = Framer::FramerateUncapped();
  ImGui::Checkbox("Uncapped Framerate", &uncappedStatus);
  if (uncappedStatus != Framer::FramerateUncapped()) {
    if (uncappedStatus == true) {
      Framer::SetFramerate(0);
    }
    else {
      Framer::SetDefaultFramerate();
    }
  }

  // Display the framerate slider.
  if (uncappedStatus == false) {
    int framerate = Framer::GetFramerate();
    ImGui::SliderInt("Framerate", &framerate, 1, 120);
    if (framerate != Framer::GetFramerate()) {
      Framer::SetFramerate(framerate);
    }
  }
  ImGui::End();
}

} // namespace Editor
