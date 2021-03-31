#ifndef editor_hook_Hook_h
#define editor_hook_Hook_h

#include "world/Object.h"

namespace Editor {
namespace Hook {

template<typename T>
T* InspectComponent(const World::Object& object);
template<typename T>
T* InspectGizmoComponent(const World::Object& object);
template<typename T>
void MakeComponentAvailable(const World::Object& object);

template<typename T>
void Edit(T* component);

struct GizmoBase
{
  virtual ~GizmoBase();
};
template<typename T>
struct Gizmo: public GizmoBase
{
  Gizmo();
  void Show(T* component);
};
void EndAllGizmos();

} // namespace Hook
} // namespace Editor

#include "Hook.hh"

#endif
