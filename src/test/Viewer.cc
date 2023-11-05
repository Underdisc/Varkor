#include <imgui/imgui.h>

#include "VarkorMain.h"
#include "debug/Draw.h"
#include "editor/Camera.h"
#include "editor/Editor.h"
#include "editor/Utility.h"
#include "editor/gizmos/Gizmos.h"
#include "editor/gizmos/Rotator.h"
#include "editor/gizmos/Scalor.h"
#include "editor/gizmos/Translator.h"
#include "test/math/Box.h"
#include "world/World.h"

namespace Test {

enum class Type
{
  BoxIntersection = 0,
  Invalid = 1,
};
const char* nTypeStrs[] = {"BoxIntersection"};

struct Desc
{
  Type mType;
  int mIdx;
};

} // namespace Test

namespace Viewer {

void Show(const Test::BoxIntersectionTest& test)
{
  Vec3 color = {1, 1, 1};
  if (Math::HasIntersection(test.mA, test.mB)) {
    color = {0, 1, 0};
  }
  Debug::Draw::Box(test.mA, color);
  Debug::Draw::Box(test.mB, color);
}

void Gizmo(Test::BoxIntersectionTest& test)
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

Ds::Vector<Test::BoxIntersectionTest> nBoxIntersectionTests;
Test::Desc nSelectedTest;
bool nShowGizmo = false;

size_t TestTypeCount(Test::Type type)
{
  switch (type) {
  case Test::Type::BoxIntersection: return nBoxIntersectionTests.Size();
  }
  return 0;
}

std::string TestName(Test::Type type, size_t idx)
{
  switch (type) {
  case Test::Type::BoxIntersection: return nBoxIntersectionTests[idx].mName;
  }
  return "Invalid";
}

std::string TestName(const Test::Desc& test)
{
  return TestName(test.mType, test.mIdx);
}

void Show(const Test::Desc& test)
{
  switch (test.mType) {
  case Test::Type::BoxIntersection:
    Show(nBoxIntersectionTests[test.mIdx]);
    break;
  }
  if (!nShowGizmo) {
    return;
  }
  switch (test.mType) {
  case Test::Type::BoxIntersection:
    Gizmo(nBoxIntersectionTests[test.mIdx]);
    break;
  }
}

void Init()
{
  nBoxIntersectionTests = Test::GetBoxIntersectionTests();
  nSelectedTest.mType = Test::Type::BoxIntersection;
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