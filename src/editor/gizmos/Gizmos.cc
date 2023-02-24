#include "editor/gizmos/Gizmos.h"
#include "editor/Editor.h"
#include "editor/gizmos/Rotator.h"
#include "editor/gizmos/Scalor.h"
#include "editor/gizmos/Translator.h"
#include "rsl/Library.h"

namespace Editor {
namespace Gizmos {

const char* nGizmoAssetName = "vres/gizmo";
ResId nArrowMeshId(nGizmoAssetName, "Arrow");
ResId nCubeMeshId(nGizmoAssetName, "Cube");
ResId nScaleMeshId(nGizmoAssetName, "Scale");
ResId nSphereMeshId(nGizmoAssetName, "Sphere");
ResId nTorusMeshId(nGizmoAssetName, "Torus");
ResId nColorShaderId("vres/renderer:Color");

void Init()
{
  Rsl::RequireAsset(nGizmoAssetName);
}

void PurgeUnneeded()
{
  TryPurge<Translator>();
  TryPurge<Scalor>();
  TryPurge<Rotator>();
}

void PurgeAll()
{
  Purge<Translator>();
  Purge<Scalor>();
  Purge<Rotator>();
}

// The Translator, Scalor, and Rotator use this to set the parent transformation
// of their handles. It's special because it scales the parent depending on the
// editor camera's distance.
void SetParentTransformation(
  World::MemberId parentId, const Vec3& translation, const Quat& referenceFrame)
{
  const World::Object cameraObject = nCamera.GetObject();
  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  float uniformScale = cameraComp.ProjectedDistance(cameraObject, translation);
  if (uniformScale == 0.0f) {
    return;
  }
  uniformScale *= 0.3f;

  Comp::Transform& parentTransform =
    nSpace.GetComponent<Comp::Transform>(parentId);
  parentTransform.SetTranslation(translation);
  parentTransform.SetUniformScale(uniformScale);
  parentTransform.SetRotation(referenceFrame);
}

} // namespace Gizmos
} // namespace Editor
