#include <glad/glad.h>

#include "Error.h"
#include "ds/Vector.h"
#include "gfx/Shader.h"
#include "math/Matrix4.h"
#include "math/Vector.h"

#include "Draw.h"

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
Gfx::Shader nShader;

void Init()
{
  Gfx::Shader::InitResult result =
    nShader.Init("vres/shader/DebugLine.vs", "vres/shader/Color.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());

  glPointSize(8.0f);
  glLineWidth(4.0f);
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
  nShader.SetMat4("uView", view.CData(), true);
  nShader.SetMat4("uProj", projection.CData(), true);
  for (int i = 0; i < nRenderables.Size(); ++i)
  {
    const Renderable& renderable = nRenderables[i];
    nShader.SetVec3("uColor", renderable.mColor.CData());
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
