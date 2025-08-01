#include <imgui/imgui.h>

#include "Input.h"
#include "Log.h"
#include "comp/Name.h"
#include "comp/Relationship.h"
#include "editor/Editor.h"
#include "editor/FileInterface.h"
#include "editor/LayerInterface.h"
#include "editor/Utility.h"
#include "gfx/Renderer.h"
#include "rsl/Library.h"
#include "world/Space.h"

namespace Editor {

LayerInterface::LayerInterface(World::LayerIt layerIt): mLayerIt(layerIt) {}

void LayerInterface::Show() {
  ImGui::Begin("Layer", &mOpen);

  // Allow the user to change the layer's camera with drag and drop.
  World::Space& space = mLayerIt->mSpace;
  World::MemberId cameraId = mLayerIt->mCameraId;
  std::stringstream cameraLabel;
  cameraLabel << "Camera: ";
  if (cameraId != World::nInvalidMemberId) {
    cameraLabel << cameraId;
    Comp::Name* nameComp = space.TryGet<Comp::Name>(cameraId);
    if (nameComp != nullptr) {
      cameraLabel << " (" << nameComp->mName << ")";
    }
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

  DropResourceIdWidget(
    Rsl::ResTypeId::Material, &mLayerIt->mPostMaterialId, "Post");

  // Display a selectable list of all members in the layer's space.
  if (ImGui::Button("Create Member", ImVec2(-1, 0))) {
    space.CreateMember();
  }
  ImGui::BeginChild("Members", ImVec2(0, 0), true);
  Ds::Vector<World::MemberId> rootMemberIds = space.RootMemberIds();
  InspectorInterface* inspector = FindInterface<InspectorInterface>();
  for (int i = 0; i < rootMemberIds.Size(); ++i) {
    DisplayMember(rootMemberIds[i], &inspector);
  }
  ImGui::EndChild();

  // Make the member window a drag drop target for ending parent relationships.
  if (ImGui::BeginDragDropTarget()) {
    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MemberId");
    if (payload != nullptr) {
      World::MemberId childId = *(const World::MemberId*)payload->Data;
      space.TryRemoveParent(childId);
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::End();
}

void LayerInterface::ObjectPicking() {
  if (!nSuppressObjectPicking && Input::MousePressed(Input::Mouse::Left)) {
    World::Space& space = mLayerIt->mSpace;
    World::MemberId clickedMemberId =
      Gfx::Renderer::HoveredMemberId(space, nCamera.GetObject());
    InspectorInterface* inspector = FindInterface<InspectorInterface>();
    bool clickedSelected =
      inspector != nullptr && clickedMemberId == inspector->mObject.mMemberId;
    if (clickedSelected) {
      CloseInterface<InspectorInterface>();
    }
    else if (clickedMemberId != World::nInvalidMemberId) {
      World::Object clickedObject(&space, clickedMemberId);
      OpenInterface<InspectorInterface>(clickedObject);
    }
  }
  nSuppressObjectPicking = false;
}

void LayerInterface::SaveLayer() {
  if (mLayerIt->mFilename == "") {
    SaveLayerAs();
  }
  else {
    SaveLayerAs(mLayerIt->mFilename);
  }
}

void LayerInterface::SaveLayerAs() {
  auto saveLayer = [this](const std::string& filename) {
    SaveLayerAs(Rsl::PrependResDirectory(filename));
  };
  OpenInterface<FileInterface>(
    saveLayer,
    FileInterface::AccessType::Save,
    mLayerIt->mName + World::nLayerExtension);
}

void LayerInterface::SaveLayerAs(const std::string& filename) {
  Result result = World::SaveLayer(mLayerIt, filename.c_str());
  std::string logString =
    "\"" + mLayerIt->mName + "\" in \"" + mLayerIt->mFilename + "\".";
  if (result.Success()) {
    logString = "Successfully saved " + logString;
  }
  else {
    logString = "Failed to save \"" + logString + "\n" + result.mError;
  }
  Log::String(logString);
}

void LayerInterface::DisplayMember(
  World::MemberId memberId, InspectorInterface** inspector) {
  // Create the Member's tree node.
  bool selected =
    *inspector != nullptr && memberId == (*inspector)->mObject.mMemberId;
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
  if (selected) {
    flags |= ImGuiTreeNodeFlags_Selected;
  }
  World::Space& space = mLayerIt->mSpace;
  if (!space.HasChildren(memberId)) {
    flags |= ImGuiTreeNodeFlags_Leaf;
  }
  ImGui::PushID(memberId);
  std::stringstream memberLabel;
  memberLabel << memberId;
  Comp::Name* nameComp = space.TryGet<Comp::Name>(memberId);
  if (nameComp != nullptr) {
    memberLabel << " (" << nameComp->mName << ")";
  }
  bool memberOpened = ImGui::TreeNodeEx(memberLabel.str().c_str(), flags);
  ImGui::PopID();

  // Make the node a source and target for parenting drag drop operations.
  if (ImGui::BeginDragDropSource()) {
    ImGui::SetDragDropPayload("MemberId", &memberId, sizeof(World::MemberId));
    ImGui::TextUnformatted(memberLabel.str().c_str());
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
  if (selected && nameComp != nullptr) {
    ImGui::PushItemWidth(-1);
    InputText("Name", &nameComp->mName);
    ImGui::PopItemWidth();
  }

  // Display all of the children if the Member's tree node is opened.
  if (memberOpened) {
    auto* relationship = space.TryGet<Comp::Relationship>(memberId);
    if (relationship != nullptr) {
      for (World::MemberId childId: relationship->mChildren) {
        DisplayMember(childId, inspector);
      }
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
