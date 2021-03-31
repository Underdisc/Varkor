#include "ds/Vector.h"
#include "editor/hook/Hook.h"

namespace Editor {
namespace Hook {

Ds::Vector<GizmoStorage> nGizmoStore;

GizmoBase::~GizmoBase() {}

void EndAllGizmos()
{
  nGizmoStore.Clear();
}

} // namespace Hook
} // namespace Editor
