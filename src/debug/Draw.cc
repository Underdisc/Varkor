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
    nShader.Init("vres/debugLine.vs", "vres/debugLine.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());
}

void Line(const Vec3& a, const Vec3& b, const Vec3& color)
{
  Vec3 vertexBuffer[2] = {a, b};
  unsigned int vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(
    GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STREAM_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

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
  nShader.SetMat4("view", view.CData(), true);
  nShader.SetMat4("proj", projection.CData(), true);
  for (int i = 0; i < nRenderables.Size(); ++i)
  {
    const Renderable& renderable = nRenderables[i];
    nShader.SetVec3("color", renderable.mColor.CData());
    glBindVertexArray(renderable.mVao);
    glDrawArrays(GL_LINES, 0, renderable.mCount);
  }

  for (int i = 0; i < nRenderables.Size(); ++i)
  {
    const Renderable& renderable = nRenderables[i];
    glDeleteVertexArrays(1, &renderable.mVao);
    glDeleteBuffers(1, &renderable.mVbo);
  }
  nRenderables.Clear();
}

} // namespace Draw
} // namespace Debug
