#include "Error.h"
#include "ds/Map.h"

namespace Editor {
namespace Hook {

template<typename T>
void CastEdit(void* component)
{
  Edit<T>((T*)component);
}

extern Ds::Map<Comp::TypeId, GizmoBase*> nGizmos;

template<typename T>
void GizmoStart()
{
  Gizmo<T>* gizmo = alloc Gizmo<T>;
  nGizmos.Insert(Comp::Type<T>::smId, (GizmoBase*)gizmo);
}

template<typename T>
bool GizmoRun(void* component, const World::Object& object)
{
  GizmoBase** base = nGizmos.Find(Comp::Type<T>::smId);
  LogAbortIf(base == nullptr, "Gizmo has not started.");
  Gizmo<T>* gizmo = (Gizmo<T>*)(*base);
  return gizmo->Run((T*)component, object);
}

} // namespace Hook
} // namespace Editor
