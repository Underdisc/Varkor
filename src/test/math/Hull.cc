#include <random>

#include "gfx/Mesh.h"
#include "math/Constants.h"
#include "math/Hull.h"
#include "test/Test.h"
#include "test/math/Hull.h"

namespace Test {

Ds::Vector<QuickHullTest> QuickHullTest::GetTests() {
  std::mt19937 generator;
  std::uniform_int_distribution<uint64_t> distribution(0);
  auto acquireRandomPoint = [&]() -> Vec3 {
    Vec3 point;
    const uint64_t cutoff = 50;
    for (int i = 0; i < 3; ++i) {
      point[i] = (float)(distribution(generator) % cutoff) / (float)cutoff;
      point[i] = point[i] * 2.0f - 1.0f;
    }
    return point;
  };

  Ds::Vector<QuickHullTest> tests;
  Ds::Vector<Vec3> points;
  points.Push({1, 1, -1});
  tests.Emplace("0", std::move(points));

  points.Push({0, 1, 1});
  points.Push({1, -1, 0});
  tests.Emplace("1", std::move(points));

  points.Push({1, 0, 1});
  points.Push({0, 1, 0});
  points.Push({0, 1, -1});
  tests.Emplace("2", std::move(points));

  points.Push({1, 1, 1});
  points.Push({-1, 1, 1});
  points.Push({-1, -1, 1});
  points.Push({-1, -1, -1});
  tests.Emplace("3", std::move(points));

  points.Push({1, 1, 1});
  points.Push({1, 1, 1});
  points.Push({-1, -1, -1});
  points.Push({-1, -1, -1});
  tests.Emplace("4", std::move(points));

  points.Push({1, 1, 1});
  points.Push({1, 1, -1});
  points.Push({-1, -1, -1});
  points.Push({-1, -1, 1});
  points.Push({0, 0, 0});
  tests.Emplace("5", std::move(points));

  float heights[4] = {-1, -0.5f, 0.5f, 1};
  for (int i = 0; i < 4; ++i) {
    points.Push({1, heights[i], -1});
    points.Push({1, heights[i], -0.5f});
    points.Push({1, heights[i], 0.5f});
    points.Push({1, heights[i], 1});
    points.Push({0.5f, heights[i], 1});
    points.Push({-0.5f, heights[i], 1});
    points.Push({-1, heights[i], 1});
    points.Push({-1, heights[i], 0.5f});
    points.Push({-1, heights[i], -0.5f});
    points.Push({-1, heights[i], -1});
    points.Push({-0.5f, heights[i], -1});
    points.Push({0.5f, heights[i], -1});
  }
  tests.Emplace("6", std::move(points));

  for (int i = 0; i < 50; ++i) {
    points.Push(acquireRandomPoint());
  }
  tests.Emplace("7", std::move(points));

  for (int i = 0; i < 10; ++i) {
    points.Push(acquireRandomPoint());
  }
  tests.Emplace("8", std::move(points));

  for (int i = 0; i < 100; ++i) {
    points.Push(acquireRandomPoint());
  }
  tests.Emplace("9", std::move(points));

  auto meshTest =
    [&tests](const char* testName, const char* meshFile, float scale) {
    VResult<Gfx::Mesh::Local> result = Gfx::Mesh::Local::Init(
      meshFile, Gfx::Mesh::Attribute::Position, false, scale);
    LogAbortIf(!result.Success(), result.mError.c_str());
    Ds::Vector<Vec3> points;
    for (int b = 0; b < result.mValue.mVertexBuffer.Size(); b += sizeof(Vec3)) {
      points.Push(*(Vec3*)&result.mValue.mVertexBuffer[b]);
    }
    tests.Emplace(testName, std::move(points));
  };
  meshTest("10", "suzanne.obj", 1.0f);
  meshTest("11", "vres/model/scale.obj", 3.0f);
  meshTest("12", "icepick.obj", 1.0f);
  meshTest("13", "vres/model/questionmarkCube.obj", 1.0f);

  for (int i = 0; i < 10; ++i) {
    float radians = ((float)i / 8.0f) * Math::nTau;
    float x = std::cosf(radians) * 2.0f;
    float y = std::sinf(radians) * 2.0f;
    points.Push({x, y, 5});
    points.Push({x, y, -5});
  }
  tests.Emplace("14", std::move(points));

  return tests;
}

void QuickHull() {
  const auto tests = QuickHullTest::GetTests();
  for (const QuickHullTest& test: tests) {
    std::cout << test.mName << ": ";
    VResult<Math::Hull> result = Math::Hull::QuickHull(test.mPoints);
    if (!result.Success()) {
      std::cout << "Failure\n";
      continue;
    }
    std::cout << "Success\n";
    const Math::Hull& hull = result.mValue;
    Result stabilityResult = hull.IsStructureStable();
    if (!stabilityResult.Success()) {
      std::cout << "  " << stabilityResult.mError << '\n';
    }

    struct VertexEdgeCount {
      Vec3 mPosition;
      int mEdgeCount;
    };
    Ds::Vector<VertexEdgeCount> vertices;
    for (const Math::Hull::Vertex& vertex: hull.mVertices) {
      int edgeCount = 0;
      Ds::List<Math::Hull::HalfEdge>::CIter currentEdge = vertex.mHalfEdge;
      do {
        ++edgeCount;
        currentEdge = currentEdge->mTwin->mNext;
      } while (vertex.mHalfEdge != currentEdge);
      vertices.Push({vertex.mPosition, edgeCount});
    }

    vertices.Sort([](const VertexEdgeCount& a, const VertexEdgeCount& b) {
      for (int i = 0; i < 3; ++i) {
        if (!Math::Near(a.mPosition[i], b.mPosition[i])) {
          return a.mPosition[i] > b.mPosition[i];
        }
      }
      return a.mPosition[0] > b.mPosition[0];
    });
    for (const VertexEdgeCount& vertex: vertices) {
      std::cout << "  " << vertex.mEdgeCount << ":" << vertex.mPosition << '\n';
    }
  }
}

} // namespace Test

#ifndef RemoveTestEntryPoint
int main() {
  using namespace Test;
  RunTest(QuickHull);
}
#endif
