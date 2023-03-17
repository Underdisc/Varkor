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

extern Ds::Vector<void (*)()> nUpdates;
extern Ds::Vector<void (*)()> nClears;

// Options for switching between gizmo types, reference frames, etc.
enum class Mode
{
  Translate,
  Scale,
  Rotate
};
enum class ReferenceFrame
{
  World,
  Parent,
  Relative,
};
extern Mode nMode;
extern ReferenceFrame nReferenceFrame;
extern bool nSnapping;
extern float nTranslateSnapInterval;
extern float nScaleSnapInterval;
extern float nRotateSnapInterval;

void Init();
void Purge();
void Update();
void ImGuiOptions();

void SetParentTransformation(
  World::MemberId parentId,
  const Vec3& translation,
  const Quat& referenceFrame);

} // namespace Gizmos

template<typename T>
struct Gizmo
{
  static Ds::Vector<T> smInstances;
  static int smNext;
  static bool smRegistered;

  static T& Next()
  {
    if (!smRegistered) {
      Gizmos::nUpdates.Push(Update);
      Gizmos::nClears.Push(Clear);
      smRegistered = true;
    }

    if (smInstances.Size() == 0) {
      T::Init();
    }
    if (smNext == smInstances.Size()) {
      smInstances.Emplace();
    }
    return smInstances[smNext++];
  }

  static void Update()
  {
    if (smNext == 0) {
      Clear();
      return;
    }
    smInstances.Resize(smNext);
    smNext = 0;
  }

  static void Clear()
  {
    if (smInstances.Size() != 0) {
      T::Purge();
    }
    smInstances.Clear();
  }
};

template<typename T>
Ds::Vector<T> Gizmo<T>::smInstances;
template<typename T>
int Gizmo<T>::smNext = 0;
template<typename T>
bool Gizmo<T>::smRegistered = false;

} // namespace Editor

#endif
