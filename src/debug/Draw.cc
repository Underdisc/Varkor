#include <glad/glad.h>

#include "Error.h"
#include "debug/Draw.h"
#include "ds/Vector.h"
#include "gfx/Mesh.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"
#include "math/Geometry.h"
#include "math/Matrix4.h"
#include "rsl/Library.h"

namespace Debug {
namespace Draw {

struct Renderable
{
  unsigned int mVao;
  unsigned int mVbo;
  unsigned int mCount;
  Vec3 mColor;
};
Ds::Vector<Renderable> nRenderables;

struct LineData
{
  Vec3 mStart;
  Vec3 mStartColor;
  Vec3 mEnd;
  Vec3 mEndColor;
};
Ds::Vector<LineData> nLines;

const char* nDebugDrawAssetName = "vres/debugDraw";
const ResId nDebugDrawShaderId(nDebugDrawAssetName, "Line");
const ResId nTbnShaderId(nDebugDrawAssetName, "Tbn");

void Init()
{
  glPointSize(12.0f);
  glLineWidth(3.0f);
}

void Point(const Vec3& point, const Vec3& color)
{
  unsigned int vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3), point.mD, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  nRenderables.Push({vao, vbo, 1, color});
}

void Line(const Vec3& a, const Vec3& b, const Vec3& color)
{
  nLines.Push({a, color, b, color});
}

void Line(const Vec3& a, const Vec3& b, const Vec3& aColor, const Vec3& bColor)
{
  nLines.Push({a, aColor, b, bColor});
}

void Plane(const Math::Plane& plane, const Vec3& color)
{
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

void CartesianAxes()
{
  Vec3 x = {1.0f, 0.0f, 0.0f};
  Vec3 y = {0.0f, 1.0f, 0.0f};
  Vec3 z = {0.0f, 0.0f, 1.0f};
  Vec3 o = {0.0f, 0.0f, 0.0f};
  Line(o, x, x);
  Line(o, y, y);
  Line(o, z, z);
}

void Render(const World::Object& cameraObject)
{
  Gfx::Shader* shader = Rsl::TryGetRes<Gfx::Shader>(nDebugDrawShaderId);
  if (shader == nullptr) {
    return;
  }
  Gfx::Renderer::InitializeUniversalUniformBuffer(cameraObject);

  shader->Use();
  for (int i = 0; i < nRenderables.Size(); ++i) {
    const Renderable& renderable = nRenderables[i];
    Vec4 fullColor = (Vec4)renderable.mColor;
    fullColor[3] = 1.0f;
    shader->SetUniform("uColor", fullColor);
    glBindVertexArray(renderable.mVao);
    if (renderable.mCount == 1) {
      glDrawArrays(GL_POINTS, 0, renderable.mCount);
    }
    else {
      glDrawArrays(GL_LINES, 0, renderable.mCount);
    }
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &renderable.mVao);
    glDeleteBuffers(1, &renderable.mVbo);
  }
  nRenderables.Clear();

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

void RenderTbnVectors(const Gfx::Collection& collection)
{
  auto* shader = Rsl::TryGetRes<Gfx::Shader>(nTbnShaderId);
  if (shader == nullptr) {
    return;
  }
  shader->Use();
  for (const Gfx::Renderable::Floater& floater : collection.mFloaters) {
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
