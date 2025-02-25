#include <glad/glad.h>

#include "Error.h"
#include "debug/Draw.h"
#include "ds/Vector.h"
#include "ext/Tracy.h"
#include "gfx/Mesh.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"
#include "math/Constants.h"
#include "math/Matrix3.h"
#include "math/Plane.h"
#include "rsl/Library.h"

namespace Debug {
namespace Draw {

struct PointData {
  Vec3 mPoint;
  Vec3 mColor;
};
Ds::Vector<PointData> nPoints;

struct LineData {
  Vec3 mStart;
  Vec3 mStartColor;
  Vec3 mEnd;
  Vec3 mEndColor;
};
Ds::Vector<LineData> nLines;

const char* nDebugDrawAssetName = "vres/debugDraw";
const ResId nDebugDrawShaderId(nDebugDrawAssetName, "Basic");
const ResId nTbnShaderId(nDebugDrawAssetName, "Tbn");

static constexpr int nCirclePointCount = 40;
static Vec3 nCirclePoints[nCirclePointCount];

void Init() {
  glPointSize(12.0f);
  glLineWidth(3.0f);

  // Initialize the array of points outlining a unit circle around the x axis.
  for (int i = 0; i < nCirclePointCount; ++i) {
    float theta = 2 * Math::nPi * (i / (float)nCirclePointCount);
    nCirclePoints[i] = {0, std::sinf(theta), std::cosf(theta)};
  }
}

void Point(const Vec3& point, const Vec3& color) {
  nPoints.Push({point, color});
}

void Line(const Vec3& a, const Vec3& b, const Vec3& color) {
  ZoneScoped;
  nLines.Push({a, color, b, color});
}

void Line(
  const Vec3& a, const Vec3& b, const Vec3& aColor, const Vec3& bColor) {
  ZoneScoped;
  nLines.Push({a, aColor, b, bColor});
}

void Plane(const Math::Plane& plane, const Vec3& color) {
  Vec3 corners[4];
  const Vec3& normal = plane.Normal();
  Vec3 planeLines[2];
  planeLines[0] = Math::Normalize(Math::PerpendicularTo(plane.Normal()));
  planeLines[1] = Math::Cross(normal, planeLines[0]);

  corners[0] = plane.mPoint + planeLines[0] + planeLines[1];
  corners[1] = plane.mPoint - planeLines[0] + planeLines[1];
  corners[2] = plane.mPoint - planeLines[0] - planeLines[1];
  corners[3] = plane.mPoint + planeLines[0] - planeLines[1];

  Line(corners[0], corners[1], color);
  Line(corners[1], corners[2], color);
  Line(corners[2], corners[3], color);
  Line(corners[3], corners[0], color);
}

void Box(const Math::Box& box, const Vec3& color) {
  Mat3 orientation;
  Math::Rotate(&orientation, box.mRotation);
  Vec3 scaledBasis[3];
  for (int i = 0; i < 3; ++i) {
    scaledBasis[i] = Math::GetBasisVector(orientation, i) * (box.mScale[i] / 2);
  }

  Vec3 corners[8];
  corners[0] = box.mCenter + scaledBasis[0] + scaledBasis[1] + scaledBasis[2];
  corners[1] = box.mCenter + scaledBasis[0] - scaledBasis[1] + scaledBasis[2];
  corners[2] = box.mCenter + scaledBasis[0] - scaledBasis[1] - scaledBasis[2];
  corners[3] = box.mCenter + scaledBasis[0] + scaledBasis[1] - scaledBasis[2];
  corners[4] = box.mCenter - scaledBasis[0] + scaledBasis[1] + scaledBasis[2];
  corners[5] = box.mCenter - scaledBasis[0] - scaledBasis[1] + scaledBasis[2];
  corners[6] = box.mCenter - scaledBasis[0] - scaledBasis[1] - scaledBasis[2];
  corners[7] = box.mCenter - scaledBasis[0] + scaledBasis[1] - scaledBasis[2];

  for (int i = 0; i < 4; ++i) {
    Line(corners[i], corners[(i + 1) % 4], color);
    Line(corners[i + 4], corners[(i + 1) % 4 + 4], color);
    Line(corners[i], corners[i + 4], color);
  }
}

void Sphere(const Math::Sphere& sphere, const Vec3& color) {
  // Draw three circles rotated to be parallel with each cartesian plane.
  Quat circleRotations[3];
  circleRotations[0] = {1, 0, 0, 0};
  circleRotations[1] = Quat::InitAngleAxis(Math::nPi * 0.5f, {0, 1, 0});
  circleRotations[2] = Quat::InitAngleAxis(Math::nPi * 0.5f, {0, 0, 1});
  for (int j = 0; j < 3; ++j) {
    const Quat& rotation = circleRotations[j];
    for (int i = 0; i < nCirclePointCount; ++i) {
      Vec3 start = rotation.Rotate(nCirclePoints[i % nCirclePointCount]);
      Vec3 end = rotation.Rotate(nCirclePoints[(i + 1) % nCirclePointCount]);
      start = start * sphere.mRadius + sphere.mCenter;
      end = end * sphere.mRadius + sphere.mCenter;
      Line(start, end, color);
    }
  }
}

void Capsule(const Math::Capsule& capsule, const Vec3& color) {
  // Draw the lines along the length of the central cylinder.
  const Vec3* centers = capsule.mCenters;
  Quat capsuleRotation = Quat::InitFromTo({1, 0, 0}, centers[1] - centers[0]);
  Vec3 rimPoints[4] = {
    {0, 0, -capsule.mRadius},
    {0, capsule.mRadius, 0},
    {0, 0, capsule.mRadius},
    {0, -capsule.mRadius, 0}};
  for (int i = 0; i < 4; ++i) {
    rimPoints[i] = capsuleRotation.Rotate(rimPoints[i]);
    Line(rimPoints[i] + centers[0], rimPoints[i] + centers[1], color);
  }

  // Draw the rims of the capsule (where the sphere caps meet the cylinder).
  for (int i = 0; i < nCirclePointCount; ++i) {
    Vec3 start = nCirclePoints[i % nCirclePointCount];
    Vec3 end = nCirclePoints[(i + 1) % nCirclePointCount];
    start = capsuleRotation.Rotate(start) * capsule.mRadius;
    end = capsuleRotation.Rotate(end) * capsule.mRadius;
    Line(start + centers[0], end + centers[0], color);
    Line(start + centers[1], end + centers[1], color);
  }

  // Draw the half circles showing the spherical caps.
  Quat halfCircleRotations[4];
  halfCircleRotations[0] = Quat::InitAngleAxis(Math::nPiO2, {0, 0, 1});
  halfCircleRotations[1] = Quat::InitAngleAxis(Math::nPiO2, {1, 0, 0});
  halfCircleRotations[1] = halfCircleRotations[1] * halfCircleRotations[0];
  halfCircleRotations[2] = Quat::InitAngleAxis(-Math::nPiO2, {0, 0, 1});
  halfCircleRotations[3] = Quat::InitAngleAxis(Math::nPiO2, {1, 0, 0});
  halfCircleRotations[3] = halfCircleRotations[3] * halfCircleRotations[2];
  for (int j = 0; j < 4; ++j) {
    const Vec3& center = centers[j / 2];
   Quat halfCircleRot = capsuleRotation * halfCircleRotations[j];
    for (int i = 0; i < nCirclePointCount / 2; ++i) {
      Vec3 start = nCirclePoints[i % nCirclePointCount];
      Vec3 end = nCirclePoints[(i + 1) % nCirclePointCount];
      start = halfCircleRot.Rotate(start) * capsule.mRadius;
      end = halfCircleRot.Rotate(end) * capsule.mRadius;
      Line(start + center, end + center, color);
    }
  }
}

void Triangle(const Math::Triangle& triangle, const Vec3& color) {
  const Vec3& a = triangle.mPoints[0];
  const Vec3& b = triangle.mPoints[1];
  const Vec3& c = triangle.mPoints[2];
  Line(a, b, color);
  Line(b, c, color);
  Line(c, a, color);
}

void CartesianAxes() {
  Vec3 x = {1.0f, 0.0f, 0.0f};
  Vec3 y = {0.0f, 1.0f, 0.0f};
  Vec3 z = {0.0f, 0.0f, 1.0f};
  Vec3 o = {0.0f, 0.0f, 0.0f};
  Line(o, x, x);
  Line(o, y, y);
  Line(o, z, z);
}

void Render(const World::Object& cameraObject) {
  ZoneScoped;

  Gfx::Shader* shader = Rsl::TryGetRes<Gfx::Shader>(nDebugDrawShaderId);
  if (shader == nullptr) {
    return;
  }
  Gfx::Renderer::InitializeUniversalUniformBuffer(cameraObject);
  shader->Use();

  unsigned int pointVao, pointVbo;
  glGenVertexArrays(1, &pointVao);
  glGenBuffers(1, &pointVbo);
  glBindVertexArray(pointVao);
  glBindBuffer(GL_ARRAY_BUFFER, pointVbo);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(PointData) * nPoints.Size(),
    nPoints.Data(),
    GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3), (void*)sizeof(Vec3));
  glEnableVertexAttribArray(1);
  glDrawArrays(GL_POINTS, 0, (GLsizei)(nPoints.Size()));
  glBindVertexArray(0);
  glDeleteBuffers(1, &pointVbo);
  glDeleteVertexArrays(1, &pointVao);
  nPoints.Clear();

  // Upload, draw, and delete the line buffer.
  unsigned int lineVao, lineVbo;
  glGenVertexArrays(1, &lineVao);
  glGenBuffers(1, &lineVbo);
  glBindVertexArray(lineVao);
  glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
  glBufferData(
    GL_ARRAY_BUFFER,
    sizeof(LineData) * nLines.Size(),
    nLines.Data(),
    GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
    1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(Vec3), (void*)sizeof(Vec3));
  glEnableVertexAttribArray(1);
  glDrawArrays(GL_LINES, 0, (GLsizei)(2 * nLines.Size()));
  glBindVertexArray(0);
  glDeleteBuffers(1, &lineVbo);
  glDeleteVertexArrays(1, &lineVao);
  nLines.Clear();
}

void RenderTbnVectors(const Gfx::Collection& collection) {
  auto* shader = Rsl::TryGetRes<Gfx::Shader>(nTbnShaderId);
  if (shader == nullptr) {
    return;
  }
  shader->Use();
  for (const Gfx::Renderable::Floater& floater: collection.mFloaters) {
    const auto* mesh = Rsl::TryGetRes<Gfx::Mesh>(floater.mMeshId);
    if (mesh == nullptr) {
      continue;
    }
    shader->SetUniform("uModel", floater.mTransform);
    mesh->Render();
  }
}

} // namespace Draw
} // namespace Debug
