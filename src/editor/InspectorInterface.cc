#include <imgui/imgui.h>

#include "Input.h"
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
  if (duplicate) {
    mObject = mObject.Duplicate();
  }

  ImGui::Begin("Inspector", &mOpen, ImGuiWindowFlags_NoFocusOnAppearing);
  if (ImGui::Button("Add Components", ImVec2(-1, 0))) {
    OpenInterface<AddComponentInterface>(mObject);
  }

  // Display all of the Member's components and the active component hooks.
  Ds::Vector<World::ComponentDescriptor> descriptors =
    mObject.mSpace->GetDescriptors(mObject.mMemberId);
  for (int i = 0; i < descriptors.Size(); ++i) {
    bool removeComponent = false;
    const World::ComponentDescriptor& desc = descriptors[i];
    const Comp::TypeData& typeData = Comp::GetTypeData(desc.mTypeId);
    bool inspecting = ImGui::CollapsingHeader(typeData.mName.c_str());
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Remove")) {
        removeComponent = true;
      }
      ImGui::EndPopup();
    }
    if (inspecting && typeData.mVEdit.Open()) {
      void* component = mObject.GetComponent(desc.mTypeId);
      typeData.mVEdit.Invoke(component, mObject);
    }
    if (removeComponent == true) {
      mObject.RemComponent(desc.mTypeId);
    }
  }
  ImGui::End();
}

AddComponentInterface::AddComponentInterface(const World::Object& object):
  mObject(object)
{}

void AddComponentInterface::Show()
{
  ImGui::Begin("Add Components", &mOpen);
  for (Comp::TypeId typeId = 0; typeId < Comp::TypeDataCount(); ++typeId) {
    if (mObject.HasComponent(typeId)) {
      continue;
    }
    const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
    if (ImGui::Button(typeData.mName.c_str(), ImVec2(-1, 0))) {
      mObject.AddComponent(typeId);
    }
  }
  ImGui::End();
}

} // namespace Editor
