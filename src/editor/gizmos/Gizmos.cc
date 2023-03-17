#include <imgui/imgui.h>

#include "Input.h"
#include "editor/Editor.h"
#include "editor/gizmos/Gizmos.h"
#include "math/Constants.h"
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

Ds::Vector<void (*)()> nUpdates;
Ds::Vector<void (*)()> nClears;

Mode nMode = Mode::Translate;
ReferenceFrame nReferenceFrame = ReferenceFrame::World;
bool nSnapping = false;
float nTranslateSnapInterval = 1.0f;
float nScaleSnapInterval = 0.5f;
float nRotateSnapInterval = Math::nPi / 4.0f;

void Init()
{
  Rsl::RequireAsset(nGizmoAssetName);
}

void Purge()
{
  for (int i = 0; i < nClears.Size(); ++i) {
    nClears[i]();
  }
}

void Update()
{
  for (int i = 0; i < nUpdates.Size(); ++i) {
    nUpdates[i]();
  }

  // Handle all hotkeys for switching between modes and reference frames.
  if (Input::KeyDown(Input::Key::LeftControl)) {
    if (Input::KeyPressed(Input::Key::Z)) {
      nReferenceFrame = ReferenceFrame::World;
    }
    else if (Input::KeyPressed(Input::Key::X)) {
      nReferenceFrame = ReferenceFrame::Parent;
    }
    else if (Input::KeyPressed(Input::Key::C)) {
      nReferenceFrame = ReferenceFrame::Relative;
    }
  }
  else {
    if (Input::KeyPressed(Input::Key::Z)) {
      nMode = Mode::Translate;
    }
    else if (Input::KeyPressed(Input::Key::X)) {
      nMode = Mode::Scale;
    }
    else if (Input::KeyPressed(Input::Key::C)) {
      nMode = Mode::Rotate;
    }
  }
}

void ImGuiOptions()
{
  // Display drop down lists for switching between modes and reference frames.
  const char* modeNames[] = {"Translate", "Scale", "Rotate"};
  const int modeNameCount = sizeof(modeNames) / sizeof(const char*);
  int intMode = (int)nMode;
  ImGui::Combo("Mode", &intMode, modeNames, modeNameCount);
  nMode = (Mode)intMode;
  const char* frameNames[] = {"World", "Parent", "Relative"};
  const int frameNameCount = sizeof(frameNames) / sizeof(const char*);
  int intFrame = (int)nReferenceFrame;
  ImGui::Combo("Reference Frame", &intFrame, frameNames, frameNameCount);
  nReferenceFrame = (ReferenceFrame)intFrame;
  ImGui::Checkbox("Snapping", &nSnapping);
  switch (nMode) {
  case Mode::Translate:
    ImGui::InputFloat("Snap Distance", &nTranslateSnapInterval, 0.1f);
    break;
  case Mode::Scale:
    ImGui::InputFloat("Snap Increment", &nScaleSnapInterval, 0.1f);
    break;
  case Mode::Rotate:
    ImGui::InputFloat("Snap Angle", &nRotateSnapInterval);
    break;
  }
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
