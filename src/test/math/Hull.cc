#include <random>

#include "math/Hull.h"
#include "test/Test.h"
#include "test/math/Hull.h"

namespace Test {

Ds::Vector<QuickHullTest> QuickHullTest::GetTests() {
  std::mt19937 generator;
  std::uniform_int_distribution<uint64_t> distribution(0);
  auto acquireRandomPoint = [&]() -> Vec3
  {
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

  for (int i = 0; i < 50; ++i) {
    points.Push(acquireRandomPoint());
  }
  tests.Emplace("6", std::move(points));

  for (int i = 0; i < 10; ++i) {
    points.Push(acquireRandomPoint());
  }
  tests.Emplace("7", std::move(points));

  for (int i = 0; i < 100; ++i) {
    points.Push(acquireRandomPoint());
  }
  tests.Emplace("8", std::move(points));

  return tests;
}

Result IsHullStructureStable(const Math::Hull& hull) {
  Ds::List<Math::Hull::Vertex>::CIter vertIt = hull.mVertices.cbegin();
  Ds::List<Math::Hull::Vertex>::CIter vertEnd = hull.mVertices.cend();
  for (; vertIt != vertEnd; ++vertIt) {
    if (vertIt != vertIt->mHalfEdge->mVertex) {
      return Result("Vertex edge has incorrect vertex reference");
    }
  }

  size_t encounteredEdges = 0;
  Ds::List<Math::Hull::Face>::CIter faceIt = hull.mFaces.cbegin();
  Ds::List<Math::Hull::Face>::CIter faceEnd = hull.mFaces.cend();
  for (; faceIt != faceEnd; ++faceIt) {
    Ds::List<Math::Hull::HalfEdge>::CIter currentEdge = faceIt->mHalfEdge;
    do {
      if (faceIt != currentEdge->mFace) {
        return Result("Edge in face's loop refers to wrong face");
      }
      if (currentEdge != currentEdge->mTwin->mTwin) {
        return Result("Twin references of twin pair aren't cyclic");
      }
      ++encounteredEdges;
    } while ((currentEdge = currentEdge->mNext) != faceIt->mHalfEdge);
  }
  if (encounteredEdges != hull.mHalfEdges.Size()) {
    return Result("Number of visited edges not equivalent to edge list size");
  }
  return Result();
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
    Result stabilityResult = IsHullStructureStable(hull);
    if (!stabilityResult.Success()) {
      std::cout << "  " << stabilityResult.mError << '\n';
    }
    for (const Math::Hull::Vertex& vertex: hull.mVertices) {
      std::cout << "  " << vertex.mPosition << '\n';
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
