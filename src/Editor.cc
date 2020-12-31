#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Framer.h"
#include "Input.h"
#include "Viewport.h"

#include "Editor.h"

namespace Editor {

bool nShowEditorWindow = false;
bool nShowFramerWindow = false;

void EditorWindow();
void FramerWindow();

void Init()
{
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(Viewport::Window(), true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void Start()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& io = ImGui::GetIO();
  Input::SetMouseFocus(!io.WantCaptureMouse);
  Input::SetKeyboardFocus(!io.WantCaptureKeyboard);

  EditorWindow();
  if (nShowFramerWindow)
  {
    FramerWindow();
  }
}

void End()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorWindow()
{
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;
  ImGui::Begin("Editor", &nShowEditorWindow, windowFlags);
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("View"))
    {
      ImGui::MenuItem("Framer", NULL, &nShowFramerWindow);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
  ImGui::End();
}

void FramerWindow()
{
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize;
  ImGui::Begin("Framer", &nShowFramerWindow, windowFlags);

  // Display the statistics.
  ImGui::Text("FPS: %f", Framer::GetAverageFps());
  ImGui::Text("Frame Usage", Framer::GetAverageFrameUsage());
  ImGui::SameLine();
  ImGui::ProgressBar(Framer::GetAverageFrameUsage(), ImVec2(0.0f, 0.0f));
  ImGui::SliderFloat(
    "Update Period", &Framer::nAverageUpdatePeriod, 0.01f, 1.0f);

  // Display the option for vsync.
  ImGui::Separator();
  bool currentVSync = Framer::VSyncEnabled();
  bool newVSync = currentVSync;
  ImGui::Checkbox("VSync", &newVSync);
  if (newVSync != currentVSync)
  {
    Framer::SetVSync(newVSync);
  }
  if (newVSync == true)
  {
    ImGui::End();
    return;
  }

  // Display the option for uncapping the framerate.
  bool currentUncapStatus = Framer::FramerateUncapped();
  bool newUncapStatus = currentUncapStatus;
  ImGui::Checkbox("Uncapped Framerate", &newUncapStatus);
  if (newUncapStatus != currentUncapStatus)
  {
    if (newUncapStatus == true)
    {
      Framer::SetFramerate(0);
    } else
    {
      Framer::SetDefaultFramerate();
    }
  }

  // Display the framerate slider.
  if (newUncapStatus == false)
  {
    int currentFramerate = Framer::GetFramerate();
    int newFramerate = currentFramerate;
    ImGui::SliderInt("Framerate", &newFramerate, 10, 120);
    if (newFramerate != currentFramerate)
    {
      Framer::SetFramerate(newFramerate);
    }
  }
  ImGui::End();
}

} // namespace Editor
