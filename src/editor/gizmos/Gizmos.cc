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
  Math::Ray cameraRay;
  const World::Object cameraObject = nCamera.GetObject();
  const auto& cameraComp = cameraObject.Get<Comp::Camera>();
  Vec3 cameraTranslation = cameraComp.WorldTranslation(cameraObject);
  Vec3 cameraForward = cameraComp.WorldForward(cameraObject);
  cameraRay.StartDirection(cameraTranslation, cameraForward);
  Vec3 projection = cameraRay.ClosestPointTo(translation);
  Vec3 projectionDistance = projection - cameraTranslation;
  if (Math::Near(projectionDistance, {0.0f, 0.0f, 0.0f})) {
    return;
  }
  Comp::Transform& parentTransform =
    nSpace.GetComponent<Comp::Transform>(parentId);
  parentTransform.SetTranslation(translation);
  parentTransform.SetUniformScale(Math::Magnitude(projectionDistance) * 0.3f);
  parentTransform.SetRotation(referenceFrame);
}

} // namespace Gizmos
} // namespace Editor
