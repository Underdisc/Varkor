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
#include "test/math/Geometry.h"
#include "test/math/Triangle.h"
#include "world/World.h"

#include "Input.h"
#include <iomanip>

namespace Viewer {

template<typename T>
void Show(const T& test)
{}

template<typename T>
void Gizmo(T* test)
{}

bool nShowGizmo = false;

template<typename T>
struct TestVector
{
  static Ds::Vector<T> smTests;

  static size_t Count()
  {
    return smTests.Size();
  }

  static const std::string& TestName(size_t idx)
  {
    return smTests[idx].mName;
  }

  static void ShowTest(size_t idx)
  {
    if (nShowGizmo) {
      Gizmo<T>(&smTests[idx]);
    }
    Show<T>(smTests[idx]);
  }
};
template<typename T>
Ds::Vector<T> TestVector<T>::smTests;

struct TestVectorTypeData
{
  size_t (*mCount)();
  const std::string& (*mTestName)(size_t idx);
  void (*mShowTest)(size_t idx);
};

enum class TestType
{
  BoxBoxIntersection = 0,
  SphereSphereIntersection = 1,
  TriangleClosestPointTo = 2,
  SphereTriangleIntersection = 3,
  Invalid = 4,
};
const char* nTypeStrs[] = {
  "BoxBoxIntersection",
  "SphereSphereIntersection",
  "TriangleClosestPointTo",
  "SphereTriangleIntersection",
};

Ds::Vector<TestVectorTypeData> nTestVectorTypeData;

template<typename T>
void RegisterTestVector(TestType type, Ds::Vector<T> (*getTests)())
{
  TestVector<T>::smTests = getTests();

  TestVectorTypeData newTypeData;
  newTypeData.mCount = TestVector<T>::Count;
  newTypeData.mTestName = TestVector<T>::TestName;
  newTypeData.mShowTest = TestVector<T>::ShowTest;

  if ((size_t)type >= nTestVectorTypeData.Size()) {
    nTestVectorTypeData.Resize((size_t)type + 1);
  }
  nTestVectorTypeData[(size_t)type] = newTypeData;
}

void PointGizmo(Vec3* point)
{
  using namespace Editor::Gizmos;
  if (nMode == Mode::Translate) {
    *point = Editor::Gizmo<Translator>::Next().Run(*point, {1, 0, 0, 0});
  }
}

void TriangleGizmo(Math::Triangle* triangle)
{
  PointGizmo(&triangle->mA);
  PointGizmo(&triangle->mB);
  PointGizmo(&triangle->mC);
}

void SphereGizmo(Math::Sphere* sphere)
{
  using namespace Editor::Gizmos;
  switch (nMode) {
  case Mode::Translate:
    sphere->mCenter =
      Editor::Gizmo<Translator>::Next().Run(sphere->mCenter, {1, 0, 0, 0});
    break;
  case Mode::Scale:
    Vec3 scale = {sphere->mRadius};
    scale =
      Editor::Gizmo<Scalor>::Next().Run(scale, sphere->mCenter, {1, 0, 0, 0});
    sphere->mRadius = scale[0];
    break;
  }
}

void BoxGizmo(Math::Box* box)
{
  using namespace Editor::Gizmos;
  const Quat& referenceFrameRotation = box->mRotation;
  switch (nMode) {
  case Mode::Translate:
    box->mCenter = Editor::Gizmo<Translator>::Next().Run(
      box->mCenter, referenceFrameRotation);
    break;
  case Mode::Scale:
    box->mScale = Editor::Gizmo<Scalor>::Next().Run(
      box->mScale, box->mCenter, referenceFrameRotation);
    break;
  case Mode::Rotate:
    box->mRotation = Editor::Gizmo<Rotator>::Next().Run(
      box->mRotation, box->mCenter, referenceFrameRotation);
    break;
  }
}

template<>
void Show<Test::BoxBoxIntersectionTest>(
  const Test::BoxBoxIntersectionTest& test)
{
  Vec3 color = {1, 1, 1};
  if (Math::HasIntersection(test.mA, test.mB)) {
    color = {0, 1, 0};
  }
  Debug::Draw::Box(test.mA, color);
  Debug::Draw::Box(test.mB, color);
}

template<>
void Gizmo<Test::BoxBoxIntersectionTest>(Test::BoxBoxIntersectionTest* test)
{
  BoxGizmo(&test->mA);
  BoxGizmo(&test->mB);
}

template<>
void Show<Test::SphereSphereIntersectionTest>(
  const Test::SphereSphereIntersectionTest& test)
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

template<>
void Gizmo<Test::SphereSphereIntersectionTest>(
  Test::SphereSphereIntersectionTest* test)
{
  SphereGizmo(&test->mA);
  SphereGizmo(&test->mB);
}

template<>
void Show<Test::TriangleClosestPointToTest>(
  const Test::TriangleClosestPointToTest& test)
{
  Debug::Draw::Triangle(test.mTriangle, {1, 1, 1});
  Debug::Draw::Point(test.mPoint, {1, 1, 1});
  Vec3 closestPoint = test.mTriangle.ClosestPointTo(test.mPoint);
  Debug::Draw::Point(closestPoint, {0, 1, 0});
  Debug::Draw::Line(test.mPoint, closestPoint, {1, 1, 1}, {0, 1, 0});

  // Write a copy and pasteable text to console to add a new unit test.
  if (Input::KeyPressed(Input::Key::O)) {
    static size_t currentTestNumber =
      nTestVectorTypeData[(size_t)TestType::TriangleClosestPointTo].mCount();
    std::cout << std::fixed << std::setprecision(4);
    // clang-format off
    std::cout << "triangle = {{"
      << test.mTriangle.mA[0] << "f,"
      << test.mTriangle.mA[1] << "f,"
      << test.mTriangle.mA[2] << "f}, {"
      << test.mTriangle.mB[0] << "f,"
      << test.mTriangle.mB[1] << "f,"
      << test.mTriangle.mB[2] << "f}, {"
      << test.mTriangle.mC[0] << "f,"
      << test.mTriangle.mC[1] << "f,"
      << test.mTriangle.mC[2] << "f}};\n"
      << "point = {"
      << test.mPoint[0] << "f,"
      << test.mPoint[1] << "f,"
      << test.mPoint[2] << "f};\n"
      << "tests.Emplace(\"" << currentTestNumber++
      <<  "\", triangle, point);\n";
    // clang-format on
  }
}

template<>
void Gizmo<Test::TriangleClosestPointToTest>(
  Test::TriangleClosestPointToTest* test)
{
  TriangleGizmo(&test->mTriangle);
  PointGizmo(&test->mPoint);
}

template<>
void Show<Test::SphereTriangleIntersectionTest>(
  const Test::SphereTriangleIntersectionTest& test)
{
  Vec3 color = {1, 1, 1};
  Math::SphereTriangle intersection =
    Math::Intersection(test.mSphere, test.mTriangle);
  if (intersection.mIntersecting) {
    color = {0, 1, 0};
    Vec3 separationDir = Math::Normalize(intersection.mSeparation);
    Vec3 sphereSurfacePoint =
      test.mSphere.mCenter - separationDir * test.mSphere.mRadius;
    Debug::Draw::Line(
      sphereSurfacePoint,
      sphereSurfacePoint + intersection.mSeparation,
      {1, 1, 1});
  }
  Debug::Draw::Sphere(test.mSphere, color);
  Debug::Draw::Triangle(test.mTriangle, color);

  // Write a copy and pasteable text to console to add a new unit test.
  if (Input::KeyPressed(Input::Key::O)) {
    std::cout << std::fixed << std::setprecision(4);
    static int currentTestNumber =
      nTestVectorTypeData[(size_t)TestType::SphereTriangleIntersection]
        .mCount();
    // clang-format off
    std::cout << "sphere = {{"
      << test.mSphere.mCenter[0] << "f,"
      << test.mSphere.mCenter[1] << "f,"
      << test.mSphere.mCenter[2] << "f}, "
      << test.mSphere.mRadius << "f};\n"
      << "triangle = {{"
      << test.mTriangle.mA[0] << "f,"
      << test.mTriangle.mA[1] << "f,"
      << test.mTriangle.mA[2] << "f}, {"
      << test.mTriangle.mB[0] << "f,"
      << test.mTriangle.mB[1] << "f,"
      << test.mTriangle.mB[2] << "f}, {"
      << test.mTriangle.mC[0] << "f,"
      << test.mTriangle.mC[1] << "f,"
      << test.mTriangle.mC[2] << "f}};\n"
      << "tests.Emplace(\"" << currentTestNumber++
      << "\", sphere, triangle);\n";
    // clang-format on
  }
}

template<>
void Gizmo<Test::SphereTriangleIntersectionTest>(
  Test::SphereTriangleIntersectionTest* test)
{
  SphereGizmo(&test->mSphere);
  TriangleGizmo(&test->mTriangle);
}

struct Desc
{
  TestType mType;
  int mIdx;
};

Desc nSelectedTest;

void Init()
{
  RegisterTestVector<Test::BoxBoxIntersectionTest>(
    TestType::BoxBoxIntersection, Test::GetBoxBoxIntersectionTests);
  RegisterTestVector<Test::SphereSphereIntersectionTest>(
    TestType::SphereSphereIntersection, Test::GetSphereSphereIntersectionTests);
  RegisterTestVector<Test::SphereTriangleIntersectionTest>(
    TestType::SphereTriangleIntersection,
    Test::GetSphereTriangleIntersectionTests);
  RegisterTestVector<Test::TriangleClosestPointToTest>(
    TestType::TriangleClosestPointTo, Test::GetTriangleClosestPointToTests);

  nSelectedTest.mType = TestType::BoxBoxIntersection;
  nSelectedTest.mIdx = 0;
}

void Update()
{
  const TestVectorTypeData& currentTypeData =
    nTestVectorTypeData[(size_t)nSelectedTest.mType];
  currentTypeData.mShowTest(nSelectedTest.mIdx);
}

void Interface()
{
  ImGui::Begin("TestSelector");

  // A combo for selecting the type of test.
  const char* comboLabel = "Type";
  const char* currentTestTypeStr = nTypeStrs[(size_t)nSelectedTest.mType];
  ImGui::PushItemWidth(-Editor::CalcBufferWidth(comboLabel));
  bool comboOpen = ImGui::BeginCombo(comboLabel, currentTestTypeStr);
  if (comboOpen) {
    for (size_t i = 0; i < (size_t)TestType::Invalid; ++i) {
      if (ImGui::Selectable(nTypeStrs[i])) {
        nSelectedTest.mType = (TestType)i;
        nSelectedTest.mIdx = 0;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();

  // A combo for selecting the specific test.
  comboLabel = "Name";
  const TestVectorTypeData& currentTypeData =
    nTestVectorTypeData[(size_t)nSelectedTest.mType];
  size_t testCount = currentTypeData.mCount();
  const std::string& selectedTestName =
    currentTypeData.mTestName(nSelectedTest.mIdx);
  ImGui::PushItemWidth(-Editor::CalcBufferWidth(comboLabel));
  comboOpen = ImGui::BeginCombo(comboLabel, selectedTestName.c_str());
  if (comboOpen) {
    for (size_t i = 0; i < testCount; ++i) {
      bool selected = nSelectedTest.mIdx == i;
      const std::string& testName = currentTypeData.mTestName(i);
      if (ImGui::Selectable(testName.c_str(), selected)) {
        nSelectedTest.mIdx = i;
      }
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();

  // Change the current test with hotkeys.
  bool atLastTest = nSelectedTest.mIdx == testCount - 1;
  if (Input::KeyPressed(Input::Key::N) && !atLastTest) {
    ++nSelectedTest.mIdx;
  }
  bool atFirstTest = nSelectedTest.mIdx == 0;
  if (Input::KeyPressed(Input::Key::B) && !atFirstTest) {
    --nSelectedTest.mIdx;
  }

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