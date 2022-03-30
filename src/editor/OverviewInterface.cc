#include <imgui/imgui.h>

#include "Input.h"
#include "editor/Editor.h"
#include "editor/OverviewInterface.h"
#include "editor/Utility.h"
#include "gfx/Renderer.h"

namespace Editor {

OverviewInterface::OverviewInterface(World::Space* space): mSpace(space) {}

void OverviewInterface::Show()
{
  // Handle object picking.
  InspectorInterface* inspector = FindInterface<InspectorInterface>();
  if (!nSuppressObjectPicking && Input::MousePressed(Input::Mouse::Left))
  {
    World::MemberId clickedMemberId =
      Gfx::Renderer::HoveredMemberId(*mSpace, nCamera.View(), nCamera.Proj());
    if (clickedMemberId != World::nInvalidMemberId)
    {
      World::Object clickedObject(mSpace, clickedMemberId);
      inspector = OpenInterface<InspectorInterface>(clickedObject);
    }
  }
  nSuppressObjectPicking = false;

  ImGui::Begin("Overview", &mOpen);

  // Allow the user to change the camera used for the space by dragging a member
  // onto the camera widget.
  std::stringstream cameraLabel;
  cameraLabel << "Camera: ";
  if (mSpace->mCameraId != World::nInvalidMemberId)
  {
    const World::Member& camera = mSpace->GetMember(mSpace->mCameraId);
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
      mSpace->mCameraId = *(const World::MemberId*)payload->Data;
    }
  }

  // Display a selectable list of all members in the space.
  if (ImGui::Button("Create Member", ImVec2(-1, 0)))
  {
    mSpace->CreateMember();
  }
  ImGui::BeginChild("Members", ImVec2(0, 0), true);
  mSpace->VisitRootMemberIds(
    [this, inspector](World::MemberId memberId)
    {
      DisplayMember(memberId, inspector);
    });
  ImGui::EndChild();

  // Make the member window a drag drop target for ending parent relationships.
  if (ImGui::BeginDragDropTarget())
  {
    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MemberId");
    if (payload != nullptr)
    {
      World::MemberId childId = *(const World::MemberId*)payload->Data;
      World::Member& member = mSpace->GetMember(childId);
      if (member.HasParent())
      {
        mSpace->RemoveParent(childId);
      }
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::End();
}

void OverviewInterface::DisplayMember(
  World::MemberId memberId, InspectorInterface* inspector)
{
  // Create the Member's tree node.
  bool selected =
    inspector != nullptr && memberId == inspector->mObject.mMemberId;
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
  if (selected)
  {
    flags |= ImGuiTreeNodeFlags_Selected;
  }
  World::Member& member = mSpace->GetMember(memberId);
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
      mSpace->MakeParent(memberId, childId);
    }
    ImGui::EndDragDropTarget();
  }

  // Open an InspectInterface for the Member if it is clicked.
  if (ImGui::IsItemClicked())
  {
    World::Object object(mSpace, memberId);
    OpenInterface<InspectorInterface>(object);
  }

  // Display a text box for changing the Member's name.
  if (selected)
  {
    ImGui::PushItemWidth(-1);
    InputText("Name", &member.mName);
    ImGui::PopItemWidth();
  }

  // Display all of the children if the Member's tree node is opened.
  if (memberOpened)
  {
    for (World::MemberId childId : member.Children())
    {
      DisplayMember(childId, inspector);
    }
    ImGui::TreePop();
  }
}

} // namespace Editor
