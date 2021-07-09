#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <sstream>

#include "Framer.h"
#include "Input.h"
#include "Temporal.h"
#include "Viewport.h"
#include "comp/Type.h"
#include "debug/Draw.h"
#include "editor/Asset.h"
#include "editor/Camera.h"
#include "editor/Util.h"
#include "editor/hook/Hook.h"
#include "gfx/Framebuffer.h"
#include "gfx/Renderer.h"
#include "world/Types.h"
#include "world/World.h"

#include "Primary.h"

namespace Editor {

Camera nCamera;
World::SpaceId nSelectedSpace = World::nInvalidSpaceId;
World::Object nSelectedObject;
bool nSuppressObjectPicking = false;

bool nEditorMode = true;
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
  Hook::EndAllGizmos();
}

void Start()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& io = ImGui::GetIO();
  Input::SetMouseFocus(!io.WantCaptureMouse);
  Input::SetKeyboardFocus(!io.WantCaptureKeyboard);
}

void Run()
{
  // Handle duplication, camera movement, and then display the editor.
  if (nSelectedObject.Valid() && Input::KeyPressed(Input::Key::R))
  {
    World::Space& space = World::GetSpace(nSelectedObject.mSpace);
    nSelectedObject.mMember = space.Duplicate(nSelectedObject.mMember);
  }
  if (nEditorMode)
  {
    nCamera.Update(Temporal::DeltaTime());
  }
  Show();
}

void Show()
{
  // Show all of the opened editor windows.
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
  AssetWindows();
  ShowUtilWindows();

  // Render the selected space.
  if (nEditorMode && nSelectedSpace != World::nInvalidSpaceId)
  {
    const World::Space& selectedSpace = World::GetSpace(nSelectedSpace);
    Gfx::Renderer::RenderModels(
      selectedSpace, nCamera.WorldToCamera(), nCamera.Position());
    Debug::Draw::CartesianAxes();
    Debug::Draw::Render(nCamera.WorldToCamera(), Viewport::Perspective());

    // Detect clicking on an object to select it.
    if (!nSuppressObjectPicking && Input::MousePressed(Input::Mouse::Left))
    {
      nSelectedObject.mMember =
        Gfx::Renderer::HoveredMemberId(selectedSpace, nCamera.WorldToCamera());
    }
    nSuppressObjectPicking = false;
  }
}

void End()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool EditorMode()
{
  return nEditorMode;
}

const Camera& GetCamera()
{
  return nCamera;
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
  ImGui::Checkbox("Editor Mode", &nEditorMode);

  // Display a button for space creation and display all of the existing spaces.
  if (ImGui::Button("Create Space", ImVec2(-1, 0)))
  {
    World::CreateSpace();
  }
  ImGui::BeginChild("Spaces", ImVec2(0, 0), true);
  for (World::SpaceVisitor visitor; !visitor.End(); visitor.Next())
  {
    World::Space& space = visitor.CurrentSpace();
    bool selected = nSelectedSpace == visitor.CurrentSpaceId();
    ImGui::PushID(visitor.CurrentSpaceId());
    if (ImGui::Selectable(space.mName.c_str(), selected))
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
      Hook::EndAllGizmos();
      nShowAddComponentWindow = false;
    }
    ImGui::PopID();
  }
  ImGui::EndChild();
  ImGui::End();
}

void DisplayMember(World::Space& space, World::MemberId memberId)
{
  // Create the member's tree node.
  World::Member& member = space.GetMember(memberId);
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
  bool selected = memberId == nSelectedObject.mMember;
  selected = selected && nSelectedSpace == nSelectedObject.mSpace;
  if (selected)
  {
    flags |= ImGuiTreeNodeFlags_Selected;
  }
  if (member.Children().Size() == 0)
  {
    flags |= ImGuiTreeNodeFlags_Leaf;
  }
  ImGui::PushID(memberId);
  bool memberOpened = ImGui::TreeNodeEx(member.mName.c_str(), flags);
  ImGui::PopID();

  // Make the node a source and target for parenting drag drop operations.
  if (ImGui::BeginDragDropSource())
  {
    ImGui::SetDragDropPayload("MemberId", &memberId, sizeof(World::MemberId));
    ImGui::Text(member.mName.c_str());
    ImGui::EndDragDropSource();
  }
  if (ImGui::BeginDragDropTarget())
  {
    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MemberId");
    if (payload != nullptr)
    {
      World::MemberId childId = *(const World::MemberId*)payload->Data;
      space.MakeParent(memberId, childId);
    }
    ImGui::EndDragDropTarget();
  }

  // Select the member if it is clicked.
  if (ImGui::IsItemClicked())
  {
    if (!selected)
    {
      nSelectedObject.mSpace = nSelectedSpace;
      nSelectedObject.mMember = memberId;
    }
    nShowAddComponentWindow = false;
  }

  // Display all of the children if the member's tree node is opened.
  if (memberOpened)
  {
    for (World::MemberId childId : member.Children())
    {
      DisplayMember(space, childId);
    }
    ImGui::TreePop();
  }
}

void OverviewWindow()
{
  // Display the text box for changing the space's name and display a button for
  // adding new members to the space.
  ImGui::Begin("Overview");
  World::Space& selectedSpace = World::GetSpace(nSelectedSpace);
  ImGui::PushItemWidth(-1);
  InputText("Name", &selectedSpace.mName);
  ImGui::PopItemWidth();

  // Allow the user to change the camera used for the space by dragging a member
  // onto the camera widget.
  std::stringstream cameraLabel;
  cameraLabel << "Camera: ";
  if (selectedSpace.mCameraId != World::nInvalidMemberId)
  {
    const World::Member& camera =
      selectedSpace.GetMember(selectedSpace.mCameraId);
    cameraLabel << camera.mName;
  } else
  {
    cameraLabel << "None" << std::endl;
  }
  ImGui::Button(cameraLabel.str().c_str(), ImVec2(-1, 0));
  if (ImGui::BeginDragDropTarget())
  {
    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MemberId");
    if (payload != nullptr)
    {
      selectedSpace.mCameraId = *(const World::MemberId*)payload->Data;
    }
  }

  if (ImGui::Button("Create Member", ImVec2(-1, 0)))
  {
    selectedSpace.CreateMember();
  }

  // Display a selectable list of all members in the space.
  ImGui::BeginChild("Members", ImVec2(0, 0), true);
  selectedSpace.VisitRootMemberIds(
    [&selectedSpace](World::MemberId memberId)
    {
      DisplayMember(selectedSpace, memberId);
    });
  ImGui::EndChild();

  // Make the member window a drag drop target for ending parent relationships.
  if (ImGui::BeginDragDropTarget())
  {
    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MemberId");
    if (payload != nullptr)
    {
      World::MemberId childId = *(const World::MemberId*)payload->Data;
      World::Member& member = selectedSpace.GetMember(childId);
      if (member.HasParent())
      {
        selectedSpace.RemoveParent(childId);
      }
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::End();
}

void InspectorWindow()
{
  // Display the inspector window.
  ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
  ImGui::PushItemWidth(-1);
  InputText("Name", &nSelectedObject.GetName());
  ImGui::PopItemWidth();
  if (ImGui::Button("Add Components", ImVec2(-1, 0)))
  {
    nShowAddComponentWindow = !nShowAddComponentWindow;
  }
  const World::Space& selectedSpace = World::GetSpace(nSelectedSpace);
  selectedSpace.VisitMemberComponents(
    nSelectedObject.mMember,
    [](Comp::TypeId typeId, int tableIndex)
    {
      Hook::InspectComponent(typeId, nSelectedObject);
    });
  ImGui::End();

  // Display the add component window.
  if (!nShowAddComponentWindow)
  {
    return;
  }
  ImGui::Begin("Add Components", &nShowAddComponentWindow);
  for (Comp::TypeId typeId = 0; typeId < Comp::nTypeData.Size(); ++typeId)
  {
    const Comp::TypeData& typeData = Comp::nTypeData[typeId];
    if (nSelectedObject.HasComponent(typeId))
    {
      continue;
    }
    if (ImGui::Button(typeData.mName.c_str(), ImVec2(-1, 0)))
    {
      nSelectedObject.AddComponent(typeId);
    }
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
