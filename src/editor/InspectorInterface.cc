#include <imgui/imgui.h>

#include "Input.h"
#include "editor/HookInterface.h"
#include "editor/InspectorInterface.h"
#include "world/World.h"

namespace Editor {

InspectorInterface::InspectorInterface(World::Object& object): mObject(object)
{}

void InspectorInterface::Show()
{
  // Perform a duplication and inspect the duplicate if requested.
  bool duplicate =
    Input::KeyDown(Input::Key::LeftControl) && Input::KeyPressed(Input::Key::D);
  if (duplicate)
  {
    mObject = mObject.Duplicate();
  }

  ImGui::Begin("Inspector", &mOpen, ImGuiWindowFlags_NoFocusOnAppearing);
  if (ImGui::Button("Add Components", ImVec2(-1, 0)))
  {
    OpenInterface<AddComponentInterface>(mObject);
  }

  // Display all of the Member's components and the active component hooks.
  World::Member& member = mObject.GetMember();
  mObject.mSpace->VisitMemberComponents(
    mObject.mMemberId,
    [this](Comp::TypeId typeId, size_t tableIndex)
    {
      bool removeComponent = false;
      const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
      const HookFunctions& hookFunctions = GetHookFunctions(typeId);
      bool inspecting = ImGui::CollapsingHeader(typeData.mName.c_str());
      if (ImGui::BeginPopupContextItem())
      {
        if (ImGui::Selectable("Remove"))
        {
          removeComponent = true;
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
        hook->Edit(mObject);
      } else if (hook != nullptr)
      {
        hookFunctions.mCloser(this);
      }
      if (removeComponent == true)
      {
        hookFunctions.mCloser(this);
        mObject.RemComponent(typeId);
      }
    });
  ImGui::End();
}

AddComponentInterface::AddComponentInterface(const World::Object& object):
  mObject(object)
{}

void AddComponentInterface::Show()
{
  ImGui::Begin("Add Components", &mOpen);
  for (Comp::TypeId typeId = 0; typeId < Comp::TypeDataCount(); ++typeId)
  {
    if (mObject.HasComponent(typeId))
    {
      continue;
    }
    const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
    if (ImGui::Button(typeData.mName.c_str(), ImVec2(-1, 0)))
    {
      mObject.AddComponent(typeId);
    }
  }
  ImGui::End();
}

} // namespace Editor
