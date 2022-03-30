#include <glad/glad.h>

#include "AssetLibrary.h"
#include "Error.h"
#include "debug/Draw.h"
#include "ds/Vector.h"
#include "gfx/Shader.h"
#include "math/Geometry.h"
#include "math/Matrix4.h"

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
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3), point.CData(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  nRenderables.Push({vao, vbo, 1, color});
}

void Line(const Vec3& a, const Vec3& b, const Vec3& color)
{
  unsigned int vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  Vec3 vertexBuffer[2] = {a, b};
  glBufferData(
    GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STREAM_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  nRenderables.Push({vao, vbo, 2, color});
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

void Render(const Mat4& view, const Mat4& projection)
{
  Gfx::Shader* shader =
    AssLib::TryGetLive<Gfx::Shader>(AssLib::nDebugDrawShaderId);
  if (shader == nullptr)
  {
    return;
  }

  GLint viewLoc = shader->UniformLocation(Gfx::Uniform::Type::View);
  GLint projLoc = shader->UniformLocation(Gfx::Uniform::Type::Proj);
  GLint alphaColorLoc = shader->UniformLocation(Gfx::Uniform::Type::AlphaColor);
  glUseProgram(shader->Id());
  glUniformMatrix4fv(viewLoc, 1, true, view.CData());
  glUniformMatrix4fv(projLoc, 1, true, projection.CData());
  for (int i = 0; i < nRenderables.Size(); ++i)
  {
    const Renderable& renderable = nRenderables[i];
    Vec4 fullColor = (Vec4)renderable.mColor;
    fullColor[3] = 1.0f;
    glUniform4fv(alphaColorLoc, 1, fullColor.CData());
    glBindVertexArray(renderable.mVao);
    if (renderable.mCount == 1)
    {
      glDrawArrays(GL_POINTS, 0, renderable.mCount);
    } else
    {
      glDrawArrays(GL_LINES, 0, renderable.mCount);
    }
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &renderable.mVao);
    glDeleteBuffers(1, &renderable.mVbo);
  }
  nRenderables.Clear();
}

} // namespace Draw
} // namespace Debug
