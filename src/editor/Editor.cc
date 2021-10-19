#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Input.h"
#include "Temporal.h"
#include "Viewport.h"
#include "editor/Camera.h"
#include "editor/CoreInterface.h"

namespace Editor {

CoreInterface nCoreInterface;
ImGuiContext* nImGuiContext;
bool nEditorMode = true;
Camera nCamera;

void Init()
{
  nImGuiContext = ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(Viewport::Window(), true);
  ImGui_ImplOpenGL3_Init("#version 330");

  // Enable the ImGui docking features.
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // Change imgui's default style so I find it more pleasing (ah opinions).
  ImGuiStyle& imStyle = ImGui::GetStyle();
  imStyle.WindowRounding = 0.0f;
  imStyle.ScrollbarRounding = 0.0f;
  imStyle.TabRounding = 0.0f;
  imStyle.FrameBorderSize = 1.0f;
  imStyle.TabBorderSize = 1.0f;
  imStyle.IndentSpacing = 11.0f;
  imStyle.WindowMenuButtonPosition = ImGuiDir_Right;

  // A theme made for nerds.
  ImVec4* colors = imStyle.Colors;
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.90f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.29f, 0.00f, 1.00f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.39f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.29f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.49f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 0.39f, 0.00f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.59f, 0.20f, 1.00f);
  colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.29f, 0.00f, 1.00f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.39f, 0.00f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.00f, 0.39f, 0.00f, 1.00f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.78f, 0.00f, 1.00f);
  colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.39f, 0.00f, 1.00f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.39f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
}

void Purge()
{
  nCoreInterface.PurgeInterfaces();
  ImGui::DestroyContext(nImGuiContext);
}

void Run()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& io = ImGui::GetIO();
  Input::SetMouseFocus(!io.WantCaptureMouse);
  Input::SetKeyboardFocus(!io.WantCaptureKeyboard);

  if (nEditorMode)
  {
    nCamera.Update(Temporal::DeltaTime());
  }
  nCoreInterface.HandleInterfaces();
}

void Render()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace Editor