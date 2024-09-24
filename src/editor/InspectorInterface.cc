#include <imgui/imgui.h>

#include "Input.h"
#include "editor/InspectorInterface.h"
#include "world/World.h"

namespace Editor {

Ds::Vector<Comp::TypeId> InspectorInterface::smOpenTypes;

InspectorInterface::InspectorInterface(World::Object& object): mObject(object)
{}

void InspectorInterface::Show()
{
  // Perform a duplication and inspect the duplicate if requested.
  if (Input::ActionPressed(Input::Action::DuplicateEntity)) {
    mObject = mObject.Duplicate();
  }

  ImGui::Begin("Inspector", &mOpen, ImGuiWindowFlags_NoFocusOnAppearing);
  if (ImGui::Button("Add Components", ImVec2(-1, 0))) {
    OpenInterface<AddComponentInterface>(mObject);
  }

  // Display all of the Member's components and the active component hooks.
  Ds::Vector<Comp::TypeId> componentTypes =
    mObject.mSpace->GetComponentTypes(mObject.mMemberId);
  for (Comp::TypeId typeId : componentTypes) {
    bool removeComponent = false;
    const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
    bool inspecting = ImGui::CollapsingHeader(typeData.mName.c_str());
    if (ImGui::BeginPopupContextItem()) {
      if (ImGui::Selectable("Remove")) {
        removeComponent = true;
      }
      ImGui::EndPopup();
    }
    VResult<size_t> findResult = smOpenTypes.Find(typeId);
    if (inspecting) {
      if (!findResult.Success()) {
        smOpenTypes.Push(typeId);
      }
      if (typeData.mVEdit.Open()) {
        void* component = mObject.GetComponent(typeId);
        typeData.mVEdit.Invoke(component, mObject);
      }
    }
    else if (findResult.Success()) {
      smOpenTypes.Remove(findResult.mValue);
    }
    if (removeComponent == true) {
      mObject.RemComponent(typeId);
    }
  }
  ImGui::End();
}

void InspectorInterface::ShowGizmos()
{
  for (Comp::TypeId typeId : smOpenTypes) {
    const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
    if (typeData.mVGizmoEdit.Open()) {
      void* component = mObject.TryGetComponent(typeId);
      if (component != nullptr) {
        typeData.mVGizmoEdit.Invoke(component, mObject);
      }
    }
  }
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
    if (ImGui::Selectable(typeData.mName.c_str())) {
      mObject.AddComponent(typeId);
    }
  }
  ImGui::End();
}

} // namespace Editor
