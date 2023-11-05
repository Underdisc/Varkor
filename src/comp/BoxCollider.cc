#include <imgui/imgui.h>

#include "comp/BoxCollider.h"
#include "debug/Draw.h"
#include "editor/Utility.h"
#include "editor/gizmos/Gizmos.h"
#include "editor/gizmos/Rotator.h"
#include "editor/gizmos/Scalor.h"
#include "editor/gizmos/Translator.h"

namespace Comp {

void BoxCollider::VInit(const World::Object& owner)
{
  mBox.mCenter = {0, 0, 0};
  mBox.mScale = {1, 1, 1};
  mBox.mRotation.Identity();
  mShow = false;
}

void BoxCollider::VSerialize(Vlk::Value& val)
{
  val("Center") = mBox.mCenter;
  val("Scale") = mBox.mScale;
  val("Rotation") = mBox.mRotation;
  val("Show") = mShow;
}

void BoxCollider::VDeserialize(const Vlk::Explorer& ex)
{
  mBox.mCenter = ex("Center").As<Vec3>({0, 0, 0});
  mBox.mScale = ex("Scale").As<Vec3>({1, 1, 1});
  mBox.mRotation = ex("Rotation").As<Quat>({1, 0, 0, 0});
  mShow = ex("Show").As<bool>(false);
}

void BoxCollider::VRenderable(const World::Object& owner)
{
  if (mShow) {
    Debug::Draw::Box(mBox, {1, 1, 1});
  }
}

void BoxCollider::VEdit(const World::Object& owner)
{
  ImGui::DragFloat3("Center", mBox.mCenter.mD, 0.01f);
  ImGui::DragFloat3("Scale", mBox.mScale.mD, 0.01f);
  Editor::RotationEdit(&mBox.mRotation);
  ImGui::Checkbox("Show", &mShow);
}

void BoxCollider::VGizmoEdit(const World::Object& owner)
{
  using namespace Editor::Gizmos;
  const Quat& referenceFrameRotation = mBox.mRotation;
  switch (nMode) {
  case Mode::Translate:
    mBox.mCenter = Editor::Gizmo<Translator>::Next().Run(
      mBox.mCenter, referenceFrameRotation);
    break;
  case Mode::Scale:
    mBox.mScale = Editor::Gizmo<Scalor>::Next().Run(
      mBox.mScale, mBox.mCenter, referenceFrameRotation);
    break;
  case Mode::Rotate:
    mBox.mRotation = Editor::Gizmo<Rotator>::Next().Run(
      mBox.mRotation, mBox.mCenter, referenceFrameRotation);
    break;
  }
}

} // namespace Comp