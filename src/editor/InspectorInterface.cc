#include <imgui/imgui.h>

#include "Input.h"
#include "editor/HookInterface.h"
#include "editor/InspectorInterface.h"
#include "world/World.h"

namespace Editor {

InspectorInterface::InspectorInterface(
  World::SpaceId spaceId, World::MemberId memberId):
  mObject(spaceId, memberId), mSuppressObjectPicking(false)
{}

void InspectorInterface::Show()
{
  // Perform a duplication and inspect the duplicate if requested.
  World::Space& space = World::GetSpace(mObject.mSpace);
  bool duplicate = mObject.Valid() && Input::KeyDown(Input::Key::LeftControl) &&
    Input::KeyPressed(Input::Key::D);
  if (duplicate)
  {
    mObject.mMember = space.Duplicate(mObject.mMember);
  }

  ImGui::Begin("Inspector", &mOpen, ImGuiWindowFlags_NoFocusOnAppearing);
  if (ImGui::Button("Add Components", ImVec2(-1, 0)))
  {
    OpenInterface<AddComponentInterface>(mObject);
  }

  // Display all of the Member's components and the active component hooks.
  mSuppressObjectPicking = false;
  World::Member& member = space.GetMember(mObject.mMember);
  space.VisitMemberComponents(
    mObject.mMember,
    [this](Comp::TypeId typeId, int tableIndex)
    {
      const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
      const HookFunctions& hookFunctions = GetHookFunctions(typeId);
      bool inspecting = ImGui::CollapsingHeader(typeData.mName.c_str());
      if (ImGui::BeginPopupContextItem())
      {
        if (ImGui::Selectable("Remove"))
        {
          if (inspecting)
          {
            hookFunctions.mCloser(this);
          }
          mObject.RemComponent(typeId);
        }
        ImGui::EndPopup();
      }
      HookInterface* hook = hookFunctions.mFinder(this);
      if (inspecting)
      {
        if (hook == nullptr)
        {
          hook = hookFunctions.mOpener(this);
        }
        mSuppressObjectPicking |= hook->Edit(mObject);
      } else if (hook != nullptr)
      {
        hookFunctions.mCloser(this);
      }
    });
  ImGui::End();
}

bool InspectorInterface::SuppressObjectPicking()
{
  return mSuppressObjectPicking;
}

AddComponentInterface::AddComponentInterface(const World::Object& object):
  mObject(object)
{}

void AddComponentInterface::Show()
{
  World::Space& space = World::GetSpace(mObject.mSpace);
  ImGui::Begin("Add Components", &mOpen);
  for (Comp::TypeId typeId = 0; typeId < Comp::TypeDataCount(); ++typeId)
  {
    if (space.HasComponent(typeId, mObject.mMember))
    {
      continue;
    }
    const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
    if (ImGui::Button(typeData.mName.c_str(), ImVec2(-1, 0)))
    {
      space.AddComponent(typeId, mObject.mMember);
    }
  }
  ImGui::End();
}

} // namespace Editor
