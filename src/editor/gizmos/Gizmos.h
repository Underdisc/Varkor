#ifndef editor_gizmo_Gizmos_h
#define editor_gizmo_Gizmos_h

#include "debug/MemLeak.h"
#include "math/Quaternion.h"
#include "math/Vector.h"
#include "rsl/ResourceId.h"
#include "world/Types.h"

namespace Editor {
namespace Gizmos {

extern ResId nArrowMeshId;
extern ResId nCubeMeshId;
extern ResId nScaleMeshId;
extern ResId nSphereMeshId;
extern ResId nTorusMeshId;
extern ResId nColorShaderId;

void Init();
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
    Gizmo<GizmoType>::smInstance = alloc GizmoType;
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

} // namespace Gizmos
} // namespace Editor

#endif
