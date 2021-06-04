#ifndef editor_hook_Hook_h
#define editor_hook_Hook_h

#include "comp/Type.h"
#include "world/Object.h"

namespace Editor {
namespace Hook {

template<typename T>
void Edit(T* component);
template<typename T>
void CastEdit(void* component);

struct GizmoBase
{
  virtual ~GizmoBase();
};
template<typename T>
struct Gizmo: public GizmoBase
{
  static void Start();
  Gizmo();
  bool Run(T* component, const World::Object& object);
};
template<typename T>
static void GizmoStart();
template<typename T>
bool GizmoRun(void* component, const World::Object& object);
void EndAllGizmos();

void InspectComponent(Comp::TypeId typeId, const World::Object& object);

} // namespace Hook
} // namespace Editor

#include "Hook.hh"

#endif
