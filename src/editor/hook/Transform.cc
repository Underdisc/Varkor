#include <imgui/imgui.h>

#include "Input.h"
#include "editor/gizmos/Rotator.h"
#include "editor/gizmos/Scalor.h"
#include "editor/gizmos/Translator.h"
#include "editor/hook/Transform.h"
#include "math/Constants.h"

namespace Editor {

Hook<Comp::Transform>::Hook():
  mMode(Mode::Translate),
  mReferenceFrame(ReferenceFrame::World),
  mSnapping(false),
  mTranslateSnapInterval(1.0f),
  mScaleSnapInterval(0.5f),
  mRotateSnapInterval(Math::nPi / 4.0f)
{}

bool Hook<Comp::Transform>::Edit(const World::Object& object)
{
  // Display all of the transformation component's parameters.
  Comp::Transform* transform = object.GetComponent<Comp::Transform>();
  Vec3 translation = transform->GetTranslation();
  ImGui::PushItemWidth(-60.0f);
  bool translationDragged =
    ImGui::DragFloat3("Translation", translation.mD, 0.01f);
  if (translationDragged)
  {
    transform->SetTranslation(translation);
  }

  Vec3 scale = transform->GetScale();
  bool scaleDragged = ImGui::DragFloat3("Scale", scale.mD, 0.01f);
  if (scaleDragged)
  {
    transform->SetScale(scale);
  }

  Quat rotation = transform->GetRotation();
  Vec3 eulerAngles = rotation.EulerAngles();
  Vec3 newAngles = eulerAngles;
  bool rotationDragged =
    ImGui::DragFloat3("Rotation", newAngles.mD, 0.01f, 0.0f, 0.0f, "%.3f");
  if (rotationDragged)
  {
    Math::Quaternion xDelta, yDelta, zDelta;
    xDelta.AngleAxis(newAngles[0], {1.0f, 0.0f, 0.0f});
    yDelta.AngleAxis(newAngles[1], {0.0f, 1.0f, 0.0f});
    zDelta.AngleAxis(newAngles[2], {0.0f, 0.0f, 1.0f});
    rotation = zDelta * yDelta * xDelta;
    transform->SetRotation(rotation);
  }

  // Handle all hotkeys for switching between modes and reference frames.
  if (Input::KeyDown(Input::Key::LeftControl))
  {
    if (Input::KeyPressed(Input::Key::Z))
    {
      mReferenceFrame = ReferenceFrame::World;
    } else if (Input::KeyPressed(Input::Key::X))
    {
      mReferenceFrame = ReferenceFrame::Parent;
    } else if (Input::KeyPressed(Input::Key::C))
    {
      mReferenceFrame = ReferenceFrame::Relative;
    }
  } else
  {
    if (Input::KeyPressed(Input::Key::Z))
    {
      mMode = Mode::Translate;
    } else if (Input::KeyPressed(Input::Key::X))
    {
      mMode = Mode::Scale;
    } else if (Input::KeyPressed(Input::Key::C))
    {
      mMode = Mode::Rotate;
    }
  }

  // Display drop down lists for switching between modes and reference frames.
  const char* modeNames[] = {"Translate", "Scale", "Rotate"};
  const int modeNameCount = sizeof(modeNames) / sizeof(const char*);
  int intMode = (int)mMode;
  ImGui::Combo("Mode", &intMode, modeNames, modeNameCount);
  mMode = (Mode)intMode;
  const char* frameNames[] = {"World", "Parent", "Relative"};
  const int frameNameCount = sizeof(frameNames) / sizeof(const char*);
  int intFrame = (int)mReferenceFrame;
  ImGui::Combo("Reference Frame", &intFrame, frameNames, frameNameCount);
  mReferenceFrame = (ReferenceFrame)intFrame;
  ImGui::Checkbox("Snapping", &mSnapping);
  switch (mMode)
  {
  case Mode::Translate:
    ImGui::InputFloat("Snap Distance", &mTranslateSnapInterval, 0.1f);
    break;
  case Mode::Scale:
    ImGui::InputFloat("Snap Increment", &mScaleSnapInterval, 0.1f);
    break;
  case Mode::Rotate:
    ImGui::InputFloat("Snap Angle", &mRotateSnapInterval);
    break;
  }
  ImGui::PopItemWidth();

  // Get a rotation depending on the current reference frame.
  Quat referenceFrameRotation;
  switch (mReferenceFrame)
  {
  case ReferenceFrame::World:
    referenceFrameRotation = {1.0f, 0.0f, 0.0f, 0.0f};
    break;
  case ReferenceFrame::Parent:
    referenceFrameRotation = transform->GetParentWorldRotation(object);
    break;
  case ReferenceFrame::Relative:
    referenceFrameRotation = transform->GetWorldRotation(object);
    break;
  }

  // Display the gizmo for the current mode that the hook is in.
  Vec3 worldTranslation = transform->GetWorldTranslation(object);
  if (mMode == Mode::Translate)
  {
    Vec3 newTranslation = Gizmos::Translate(
      worldTranslation,
      referenceFrameRotation,
      mSnapping,
      mTranslateSnapInterval);
    if (!Math::Near(newTranslation, worldTranslation))
    {
      transform->SetWorldTranslation(newTranslation, object);
    }
  } else if (mMode == Mode::Scale)
  {
    referenceFrameRotation = transform->GetWorldRotation(object);
    Vec3 newScale = Gizmos::Scale(
      scale,
      worldTranslation,
      referenceFrameRotation,
      mSnapping,
      mScaleSnapInterval);
    if (!Math::Near(newScale, scale))
    {
      transform->SetScale(newScale);
    }
  } else
  {
    Quat worldRotation = transform->GetWorldRotation(object);
    Quat newWorldRotation = Gizmos::Rotate(
      worldRotation,
      worldTranslation,
      referenceFrameRotation,
      mSnapping,
      mRotateSnapInterval);
    if (!Math::Near(worldRotation.mVec, newWorldRotation.mVec))
    {
      transform->SetWorldRotation(newWorldRotation, object);
    }
  }
  return false;
}

} // namespace Editor
