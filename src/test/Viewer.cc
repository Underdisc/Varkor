#include <imgui/imgui.h>

#include "../math/Geometry.h"
#include "VarkorMain.h"
#include "debug/Draw.h"
#include "editor/Camera.h"
#include "editor/Editor.h"
#include "editor/Utility.h"
#include "editor/gizmos/Gizmos.h"
#include "editor/gizmos/Rotator.h"
#include "editor/gizmos/Scalor.h"
#include "editor/gizmos/Translator.h"
#include "world/World.h"

namespace Test {

enum class Type
{
  BoxBoxIntersection = 0,
  SphereSphereIntersection = 1,
  Invalid = 2,
};
const char* nTypeStrs[] = {"BoxBoxIntersection", "SphereSphereIntersection"};

struct Desc
{
  Type mType;
  int mIdx;
};

} // namespace Test

namespace Viewer {

void Show(const Test::BoxBoxIntersectionTest& test)
{
  Vec3 color = {1, 1, 1};
  if (Math::HasIntersection(test.mA, test.mB)) {
    color = {0, 1, 0};
  }
  Debug::Draw::Box(test.mA, color);
  Debug::Draw::Box(test.mB, color);
}

void Gizmo(Test::BoxBoxIntersectionTest& test)
{
  auto boxGizmo = [](Math::Box& box)
  {
    using namespace Editor::Gizmos;
    const Quat& referenceFrameRotation = box.mRotation;
    switch (nMode) {
    case Mode::Translate:
      box.mCenter = Editor::Gizmo<Translator>::Next().Run(
        box.mCenter, referenceFrameRotation);
      break;
    case Mode::Scale:
      box.mScale = Editor::Gizmo<Scalor>::Next().Run(
        box.mScale, box.mCenter, referenceFrameRotation);
      break;
    case Mode::Rotate:
      box.mRotation = Editor::Gizmo<Rotator>::Next().Run(
        box.mRotation, box.mCenter, referenceFrameRotation);
      break;
    }
  };
  boxGizmo(test.mA);
  boxGizmo(test.mB);
}

void Show(const Test::SphereSphereIntersectionTest& test)
{
  Vec3 color = {1, 1, 1};
  Math::SphereSphere result = Math::Intersection(test.mA, test.mB);
  if (result.mIntersecting) {
    color = {0, 1, 0};
    Vec3 separationDir = Math::Normalize(result.mSeparation);
    Vec3 start = test.mB.mCenter - test.mB.mRadius * separationDir;
    Vec3 end = start + result.mSeparation;
    Debug::Draw::Line(start, end, {1, 1, 1});
  }
  Debug::Draw::Sphere(test.mA, color);
  Debug::Draw::Sphere(test.mB, color);
}

void Gizmo(Test::SphereSphereIntersectionTest& test)
{
  auto sphereGizmo = [](Math::Sphere& sphere)
  {
    using namespace Editor::Gizmos;
    switch (nMode) {
    case Mode::Translate:
      sphere.mCenter =
        Editor::Gizmo<Translator>::Next().Run(sphere.mCenter, {1, 0, 0, 0});
      break;
    case Mode::Scale:
      Vec3 scale = {sphere.mRadius};
      scale =
        Editor::Gizmo<Scalor>::Next().Run(scale, sphere.mCenter, {1, 0, 0, 0});
      sphere.mRadius = scale[0];
      break;
    }
  };
  sphereGizmo(test.mA);
  sphereGizmo(test.mB);
}

Ds::Vector<Test::BoxBoxIntersectionTest> nBoxBoxIntersectionTests;
Ds::Vector<Test::SphereSphereIntersectionTest> nSphereSphereIntersectionTests;
Test::Desc nSelectedTest;
bool nShowGizmo = false;

size_t TestTypeCount(Test::Type type)
{
  switch (type) {
  case Test::Type::BoxBoxIntersection: return nBoxBoxIntersectionTests.Size();
  case Test::Type::SphereSphereIntersection:
    return nSphereSphereIntersectionTests.Size();
  }
  return 0;
}

std::string TestName(Test::Type type, size_t idx)
{
  switch (type) {
  case Test::Type::BoxBoxIntersection:
    return nBoxBoxIntersectionTests[idx].mName;
  case Test::Type::SphereSphereIntersection:
    return nSphereSphereIntersectionTests[idx].mName;
  }
  return "Invalid";
}

std::string TestName(const Test::Desc& test)
{
  return TestName(test.mType, test.mIdx);
}

void Show(const Test::Desc& test)
{
  if (nShowGizmo) {
    switch (test.mType) {
    case Test::Type::BoxBoxIntersection:
      Gizmo(nBoxBoxIntersectionTests[test.mIdx]);
      break;
    case Test::Type::SphereSphereIntersection:
      Gizmo(nSphereSphereIntersectionTests[test.mIdx]);
      break;
    }
  }
  switch (test.mType) {
  case Test::Type::BoxBoxIntersection:
    Show(nBoxBoxIntersectionTests[test.mIdx]);
    break;
  case Test::Type::SphereSphereIntersection:
    Show(nSphereSphereIntersectionTests[test.mIdx]);
    break;
  }
}

void Init()
{
  nBoxBoxIntersectionTests = Test::GetBoxBoxIntersectionTests();
  nSphereSphereIntersectionTests = Test::GetSphereSphereIntersectionTests();
  nSelectedTest.mType = Test::Type::BoxBoxIntersection;
  nSelectedTest.mIdx = 0;
}

void Update()
{
  Show(nSelectedTest);
}

void Interface()
{
  ImGui::Begin("TestSelector");

  // A combo for selecting the type of test.
  const char* comboLabel = "Type";
  const char* currentTestTypeStr = Test::nTypeStrs[(size_t)nSelectedTest.mType];
  ImGui::PushItemWidth(-Editor::CalcBufferWidth(comboLabel));
  bool comboOpen = ImGui::BeginCombo(comboLabel, currentTestTypeStr);
  if (comboOpen) {
    for (size_t i = 0; i < (size_t)Test::Type::Invalid; ++i) {
      if (ImGui::Selectable(Test::nTypeStrs[i])) {
        nSelectedTest.mType = (Test::Type)i;
        nSelectedTest.mIdx = 0;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();

  // A combo for selecting the specific test.
  comboLabel = "Name";
  ImGui::PushItemWidth(-Editor::CalcBufferWidth(comboLabel));
  comboOpen = ImGui::BeginCombo(comboLabel, TestName(nSelectedTest).c_str());
  if (comboOpen) {
    size_t testCount = TestTypeCount(nSelectedTest.mType);
    for (size_t i = 0; i < testCount; ++i) {
      bool selected = nSelectedTest.mIdx == i;
      std::stringstream testLabel;
      testLabel << i;
      bool selectableClicked =
        ImGui::Selectable(TestName(nSelectedTest.mType, i).c_str(), selected);
      if (selectableClicked) {
        nSelectedTest.mIdx = i;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();

  ImGui::Checkbox("Gizmo", &nShowGizmo);
  ImGui::End();
}

} // namespace Viewer

void main(int argc, char* argv[])
{
  Viewer::Init();

  Options::Config config;
  config.mWindowName = "Unit Test Viewer";
  config.mEditorLevel = Options::EditorLevel::Simple;
  VarkorInit(argc, argv, std::move(config));

  World::nPause = false;
  World::nCentralUpdate = Viewer::Update;
  Editor::nExtension = Viewer::Interface;
  VarkorRun();
  VarkorPurge();
}