#include <imgui/imgui.h>

#include "comp/Type.h"
#include "ds/Map.h"
#include "editor/Primary.h"
#include "editor/hook/Hook.h"

namespace Editor {
namespace Hook {

Ds::Map<Comp::TypeId, GizmoBase*> nGizmos;

GizmoBase::~GizmoBase() {}

void EndAllGizmos()
{
  for (const Ds::KvPair<Comp::TypeId, GizmoBase*>& gizmoPair : nGizmos)
  {
    delete gizmoPair.mValue;
  }
  nGizmos.Clear();
}

void InspectComponent(Comp::TypeId typeId, const World::Object& object)
{
  void* component = object.GetComponent(typeId);
  if (component == nullptr)
  {
    return;
  }
  const Comp::TypeData& typeData = Comp::GetTypeData(typeId);
  bool inspecting = ImGui::CollapsingHeader(typeData.mName.c_str());
  if (ImGui::BeginPopupContextItem())
  {
    if (ImGui::Selectable("Remove"))
    {
      object.RemComponent(typeId);
    }
    ImGui::EndPopup();
  }
  if (inspecting && typeData.mEditHook != nullptr)
  {
    typeData.mEditHook(component);
  }

  if (typeData.mGizmoStart == nullptr)
  {
    return;
  }
  if (inspecting)
  {
    if (!nGizmos.Contains(typeId))
    {
      typeData.mGizmoStart();
    }
    bool gizmoEditing = typeData.mGizmoRun(component, object);
    nSuppressObjectPicking |= gizmoEditing;
  } else if (nGizmos.Contains(typeId))
  {
    GizmoBase* gizmo = (GizmoBase*)(*nGizmos.Find(typeId));
    delete gizmo;
    nGizmos.Remove(typeId);
  }
}

} // namespace Hook
} // namespace Editor
