#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "viewport.h"

#include "editor.h"

namespace Editor {

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
}

void End()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace Editor
