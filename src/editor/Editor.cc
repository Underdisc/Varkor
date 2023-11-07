#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Input.h"
#include "Options.h"
#include "Viewport.h"
#include "editor/Camera.h"
#include "editor/CoreInterface.h"
#include "editor/LayerInterface.h"
#include "editor/gizmos/Gizmos.h"
#include "rsl/Library.h"
#include "world/World.h"

namespace Editor {

CoreInterface nCoreInterface;
ImGuiContext* nImGuiContext;
bool nSuppressObjectPicking = false;
bool nPlayMode = false;
bool nHideInterface = false;
World::Space nSpace;
void (*nExtension)() = nullptr;

void SetStyle()
{
  // Change imgui's default style so I find it more pleasing (ah opinions).
  ImGuiStyle& imStyle = ImGui::GetStyle();
  imStyle.WindowRounding = 0.0f;
  imStyle.ScrollbarRounding = 0.0f;
  imStyle.TabRounding = 0.0f;
  imStyle.FrameBorderSize = 1.0f;
  imStyle.TabBorderSize = 0.0f;
  imStyle.IndentSpacing = 11.0f;
  imStyle.WindowMenuButtonPosition = ImGuiDir_Right;

  // A theme made for nerds.
  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.90f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.29f, 0.00f, 1.00f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.39f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.29f, 0.00f, 1.00f);
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
  colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.78f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.39f, 0.00f, 1.00f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
  colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.00f, 0.59f, 0.00f, 1.00f);
  colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void StartImGuiFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void EndImGuiFrame()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Init()
{
  Gizmos::Init();

  // Initialize imgui.
  nImGuiContext = ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  ImGui_ImplGlfw_InitForOpenGL(Viewport::Window(), true);
  ImGui_ImplOpenGL3_Init("#version 330");
  SetStyle();

  nCamera.Init();

  // Load in the requested layers.
  World::LayerIt loadedLayerIt = World::nLayers.end();
  for (std::string layerFile : Options::nConfig.mLoadLayers) {
    layerFile = Rsl::PrependResDirectory(layerFile);
    VResult<World::LayerIt> result = World::LoadLayer(layerFile.c_str());
    if (!result.Success()) {
      LogError(result.mError.c_str());
    }
    else if (loadedLayerIt != World::nLayers.end()) {
      loadedLayerIt = result.mValue;
    }
  }

  if (Options::nConfig.mEditorLevel == Options::EditorLevel::Complete) {
    // Initialize the interfaces that are instantly opened.
    StartImGuiFrame();
    nCoreInterface.Init();
    EndImGuiFrame();
    if (loadedLayerIt != World::nLayers.end()) {
      nCoreInterface.OpenInterface<LayerInterface>(loadedLayerIt);
    }
  }
}

void Purge()
{
  nCoreInterface.PurgeInterfaces();
  Gizmos::Purge();
  nCamera.Purge();
  nSpace.Clear();
  ImGui::DestroyContext(nImGuiContext);
}

void StartFrame()
{
  Gizmos::Update();
  nCamera.Update();
  StartImGuiFrame();
  const ImGuiIO& io = ImGui::GetIO();
  Input::SetMouseFocus(!io.WantCaptureMouse);
  Input::SetKeyboardFocus(!io.WantCaptureKeyboard);
}

void RunInWorlds()
{
  auto* layerInterface = nCoreInterface.FindInterface<LayerInterface>();
  if (layerInterface == nullptr) {
    return;
  }

  auto* inspectorInterface =
    layerInterface->FindInterface<InspectorInterface>();
  if (inspectorInterface != nullptr) {
    inspectorInterface->ShowGizmos();
  }
  layerInterface->ObjectPicking();
}

void TrySaveLayer()
{
  if (nPlayMode) {
    return;
  }
  bool leftCtrl = Input::KeyDown(Input::Key::LeftControl);
  bool s = Input::KeyPressed(Input::Key::S);
  if (leftCtrl && s) {
    auto* layerInterface = nCoreInterface.FindInterface<LayerInterface>();
    if (layerInterface != nullptr) {
      layerInterface->SaveLayer();
    }
  }
}

void EndFrame()
{
  if (Options::nConfig.mEditorLevel == Options::EditorLevel::Complete) {
    TrySaveLayer();
    nCoreInterface.HandleStaging();
    RunInWorlds();
  }

  if (Input::KeyPressed(Input::Key::GraveAccent)) {
    nHideInterface = !nHideInterface;
  }

  if (!nHideInterface) {
    ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), flags);
    if (Options::nConfig.mEditorLevel == Options::EditorLevel::Complete) {
      nCoreInterface.ShowAll();
    }
    if (nExtension != nullptr) {
      nExtension();
    }
  }

  EndImGuiFrame();
}

} // namespace Editor
