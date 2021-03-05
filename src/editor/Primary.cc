#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <sstream>

#include "Framer.h"
#include "Input.h"
#include "Viewport.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "editor/Asset.h"
#include "editor/Util.h"
#include "world/Types.h"
#include "world/World.h"

#include "Primary.h"

namespace Editor {

void (*InspectComponents)(const World::Object& selected) = nullptr;
void (*AvailableComponents)(const World::Object& selected) = nullptr;

World::SpaceId nSelectedSpace = World::nInvalidSpaceId;
World::Object nSelectedObject;

bool nShowEditorWindow = false;
bool nShowFramerWindow = false;
bool nShowAddComponentWindow = false;

void EditorWindow();
void OverviewWindow();
void InspectorWindow();
void FramerWindow();

void Init()
{
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(Viewport::Window(), true);
  ImGui_ImplOpenGL3_Init("#version 330");

  // Change imgui's default style so I find it more pleasing (ah opinions).
  ImGuiStyle& imStyle = ImGui::GetStyle();
  imStyle.WindowRounding = 0.0f;
  imStyle.ScrollbarRounding = 0.0f;
  imStyle.TabRounding = 0.0f;
  imStyle.FrameBorderSize = 1.0f;
  imStyle.TabBorderSize = 1.0f;
  imStyle.WindowMenuButtonPosition = ImGuiDir_Right;

  // A theme made for nerds.
  ImVec4* colors = imStyle.Colors;
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.90f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.78f);
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
}

void Start()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& io = ImGui::GetIO();
  Input::SetMouseFocus(!io.WantCaptureMouse);
  Input::SetKeyboardFocus(!io.WantCaptureKeyboard);

  ImGui::ShowDemoWindow();

  EditorWindow();
  if (nSelectedSpace != World::nInvalidSpaceId)
  {
    OverviewWindow();
  }
  if (nSelectedObject.Valid())
  {
    InspectorWindow();
  }
  if (nShowFramerWindow)
  {
    FramerWindow();
  }
  ShowAssetWindows();
  ShowUtilWindows();
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
  ImGui::BeginMenuBar();
  if (ImGui::BeginMenu("View"))
  {
    ImGui::MenuItem("Framer", NULL, &nShowFramerWindow);
    ImGui::MenuItem("Asset", NULL, &nShowAssetWindow);
    ImGui::EndMenu();
  }
  ImGui::EndMenuBar();

  // Display a button for space creation and display all of the existing spaces.
  if (ImGui::Button("Create Space", ImVec2(-1, 0)))
  {
    World::CreateSpace();
  }
  ImGui::BeginChild("Spaces", ImVec2(0, 0), true);
  int iteration = 0;
  for (World::SpaceVisitor visitor; !visitor.End(); visitor.Next())
  {
    World::Space& space = visitor.CurrentSpace();
    bool selected = nSelectedSpace == visitor.CurrentSpaceId();
    std::stringstream label;
    label << iteration << ": " << space.mName;
    if (ImGui::Selectable(label.str().c_str(), selected))
    {
      if (selected)
      {
        nSelectedSpace = World::nInvalidSpaceId;
      } else
      {
        nSelectedSpace = visitor.CurrentSpaceId();
      }

      // The selected object is no longer valid because the space selection has
      // been changed in some way.
      nSelectedObject.Invalidate();
      nShowAddComponentWindow = false;
    }
    ++iteration;
  }
  ImGui::EndChild();
  ImGui::End();
}

void OverviewWindow()
{
  // Display the text box for changing the space's name and display a button for
  // adding new members to the space.
  ImGui::Begin("Overview");
  World::Space& space = World::GetSpace(nSelectedSpace);
  ImGui::PushItemWidth(-1);
  InputText("Name", &space.mName);
  if (ImGui::Button("Create Member", ImVec2(-1, 0)))
  {
    space.CreateMember();
  }

  // Display a selectable list of all members in the space.
  ImGui::BeginChild("Members", ImVec2(0, 0), true);
  int iteration = 0;
  for (World::Space::MemberVisitor visitor = space.CreateMemberVisitor();
       !visitor.End();
       visitor.Next())
  {
    World::Member& member = visitor.CurrentMember();
    bool selected = visitor.CurrentMemberId() == nSelectedObject.mMember &&
      nSelectedSpace == nSelectedObject.mSpace;
    std::stringstream label;
    label << iteration << ": " << member.mName;
    if (ImGui::Selectable(label.str().c_str(), selected))
    {
      if (selected)
      {
        nSelectedObject.Invalidate();
      } else
      {
        nSelectedObject.mSpace = nSelectedSpace;
        nSelectedObject.mMember = visitor.CurrentMemberId();
      }
      nShowAddComponentWindow = false;
    }
    ++iteration;
  }
  ImGui::EndChild();
  ImGui::End();
}

void InspectorWindow()
{
  // Display the inspector window.
  ImGui::Begin("Inspector", nullptr);
  ImGui::PushItemWidth(-1);
  InputText("Name", &nSelectedObject.GetName());
  if (ImGui::Button("Add Components", ImVec2(-1, 0)))
  {
    nShowAddComponentWindow = !nShowAddComponentWindow;
  }
  InspectComponent<Comp::Model>(nSelectedObject);
  InspectComponent<Comp::Transform>(nSelectedObject);
  if (InspectComponents != nullptr)
  {
    InspectComponents(nSelectedObject);
  }
  ImGui::End();

  // Display the add component window.
  if (!nShowAddComponentWindow)
  {
    return;
  }
  ImGui::Begin("Add Components", &nShowAddComponentWindow);
  MakeComponentAvailable<Comp::Model>(nSelectedObject);
  MakeComponentAvailable<Comp::Transform>(nSelectedObject);
  if (AvailableComponents != nullptr)
  {
    AvailableComponents(nSelectedObject);
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
