#include <imgui/imgui.h>

#include "Input.h"
#include "editor/Editor.h"
#include "editor/OverviewInterface.h"
#include "editor/Utility.h"
#include "gfx/Renderer.h"
#include "world/Space.h"

namespace Editor {

OverviewInterface::OverviewInterface(World::LayerIt layerIt): mLayerIt(layerIt)
{}

void OverviewInterface::Show()
{
  // Handle object picking.
  World::Space& space = mLayerIt->mSpace;
  InspectorInterface* inspector = FindInterface<InspectorInterface>();
  if (!nSuppressObjectPicking && Input::MousePressed(Input::Mouse::Left)) {
    World::MemberId clickedMemberId =
      Gfx::Renderer::HoveredMemberId(space, nCamera.View(), nCamera.Proj());
    if (clickedMemberId != World::nInvalidMemberId) {
      World::Object clickedObject(&space, clickedMemberId);
      inspector = OpenInterface<InspectorInterface>(clickedObject);
    }
  }
  nSuppressObjectPicking = false;

  ImGui::Begin("Overview", &mOpen);

  // Allow the user to change the layer's camera with drag and drop.
  std::stringstream cameraLabel;
  cameraLabel << "Camera: ";
  if (mLayerIt->mCameraId != World::nInvalidMemberId) {
    const World::Member& camera = space.GetMember(mLayerIt->mCameraId);
    cameraLabel << camera.mName;
  }
  else {
    cameraLabel << "None" << std::endl;
  }
  ImGui::Button(cameraLabel.str().c_str(), ImVec2(-1, 0));
  if (ImGui::BeginDragDropTarget()) {
    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MemberId");
    if (payload != nullptr) {
      mLayerIt->mCameraId = *(const World::MemberId*)payload->Data;
    }
  }

  // Display a selectable list of all members in the layer's space.
  if (ImGui::Button("Create Member", ImVec2(-1, 0))) {
    space.CreateMember();
  }
  ImGui::BeginChild("Members", ImVec2(0, 0), true);
  Ds::Vector<World::MemberId> rootMemberIds = space.RootMemberIds();
  for (int i = 0; i < rootMemberIds.Size(); ++i) {
    DisplayMember(rootMemberIds[i], &inspector);
  }
  ImGui::EndChild();

  // Make the member window a drag drop target for ending parent relationships.
  if (ImGui::BeginDragDropTarget()) {
    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MemberId");
    if (payload != nullptr) {
      World::MemberId childId = *(const World::MemberId*)payload->Data;
      World::Member& member = space.GetMember(childId);
      if (member.HasParent()) {
        space.RemoveParent(childId);
      }
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::End();
}

void OverviewInterface::DisplayMember(
  World::MemberId memberId, InspectorInterface** inspector)
{
  // Create the Member's tree node.
  bool selected =
    *inspector != nullptr && memberId == (*inspector)->mObject.mMemberId;
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
  if (selected) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }
  World::Space& space = mLayerIt->mSpace;
  World::Member& member = space.GetMember(memberId);
  if (member.Children().Size() == 0) {
    flags |= ImGuiTreeNodeFlags_Leaf;
  }
  ImGui::PushID(memberId);
  bool memberOpened = ImGui::TreeNodeEx(member.mName.c_str(), flags);
  ImGui::PopID();

  // Make the node a source and target for parenting drag drop operations.
  if (ImGui::BeginDragDropSource()) {
    ImGui::SetDragDropPayload("MemberId", &memberId, sizeof(World::MemberId));
    ImGui::Text(member.mName.c_str());
    ImGui::EndDragDropSource();
  }
  if (ImGui::BeginDragDropTarget()) {
    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MemberId");
    if (payload != nullptr) {
      World::MemberId childId = *(const World::MemberId*)payload->Data;
      space.MakeParent(memberId, childId);
    }
    ImGui::EndDragDropTarget();
  }

  // Open an InspectInterface for the Member if it is clicked.
  if (ImGui::IsItemClicked()) {
    World::Object object(&space, memberId);
    if (!selected) {
      *inspector = OpenInterface<InspectorInterface>(object);
    }
    else {
      CloseInterface<InspectorInterface>();
      *inspector = nullptr;
    }
  }

  // Determine whether the user wants to delete the member.
  bool deleteMember = false;
  if (ImGui::BeginPopupContextItem()) {
    if (ImGui::Selectable("Delete")) {
      deleteMember = true;
    }
    ImGui::EndPopup();
  }

  // Display a text box for changing the Member's name.
  if (selected) {
    ImGui::PushItemWidth(-1);
    InputText("Name", &member.mName);
    ImGui::PopItemWidth();
  }

  // Display all of the children if the Member's tree node is opened.
  if (memberOpened) {
    for (World::MemberId childId : member.Children()) {
      DisplayMember(childId, inspector);
    }
    ImGui::TreePop();
  }

  if (deleteMember) {
    space.DeleteMember(memberId);
    // We must close the inspector if it's displaying the now deleted member.
    if (selected) {
      CloseInterface<InspectorInterface>();
      *inspector = nullptr;
    }
    return;
  }
}

} // namespace Editor
