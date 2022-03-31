#ifndef editor_gizmo_Gizmos_h
#define editor_gizmo_Gizmos_h

#include "math/Quaternion.h"
#include "math/Vector.h"
#include "world/Types.h"

namespace Editor::Gizmos {

void PurgeUnneeded();
void PurgeAll();

void SetParentTransformation(
  World::MemberId parentId,
  const Vec3& translation,
  const Quat& referenceFrame);

template<typename GizmoType>
struct Gizmo
{
  static GizmoType* smInstance;
  // This tracks whether the gizmo should be deleted during the next purge
  // attempt (call to TryPurge).
  static bool smDestruct;
};
template<typename GizmoType>
GizmoType* Gizmo<GizmoType>::smInstance = nullptr;
template<typename GizmoType>
bool Gizmo<GizmoType>::smDestruct = false;

template<typename GizmoType>
GizmoType* GetInstance()
{
  if (Gizmo<GizmoType>::smInstance == nullptr) {
    Gizmo<GizmoType>::smInstance = new GizmoType;
  }
  // We prevent gizmo deletion during the next purge attempt.
  Gizmo<GizmoType>::smDestruct = false;
  return Gizmo<GizmoType>::smInstance;
}

template<typename GizmoType>
void Purge()
{
  if (Gizmo<GizmoType>::smInstance != nullptr) {
    delete Gizmo<GizmoType>::smInstance;
    Gizmo<GizmoType>::smInstance = nullptr;
    Gizmo<GizmoType>::smDestruct = false;
  }
}

template<typename GizmoType>
void TryPurge()
{
  if (Gizmo<GizmoType>::smDestruct) {
    Purge<GizmoType>();
  }
  // If the gizmo instance is not requested between now and the next purge
  // attempt, we want it deleted.
  Gizmo<GizmoType>::smDestruct = true;
}

} // namespace Editor::Gizmos

#endif
