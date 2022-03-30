#include "editor/gizmos/Gizmos.h"
#include "editor/Editor.h"
#include "editor/gizmos/Rotator.h"
#include "editor/gizmos/Scalor.h"
#include "editor/gizmos/Translator.h"

namespace Editor::Gizmos {

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
  cameraRay.StartDirection(nCamera.Position(), nCamera.Forward());
  Vec3 projection = cameraRay.ClosestPointTo(translation);
  Vec3 projectionDistance = projection - nCamera.Position();
  if (Math::Near(projectionDistance, {0.0f, 0.0f, 0.0f}))
  {
    return;
  }
  Comp::Transform& parentTransform =
    *nSpace.GetComponent<Comp::Transform>(parentId);
  parentTransform.SetTranslation(translation);
  parentTransform.SetUniformScale(Math::Magnitude(projectionDistance) * 0.3f);
  parentTransform.SetRotation(referenceFrame);
}

} // namespace Editor::Gizmos
