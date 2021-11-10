#include <glad/glad.h>

#include "AssetLibrary.h"
#include "Framer.h"
#include "Input.h"
#include "Viewport.h"
#include "comp/Model.h"
#include "comp/Sprite.h"
#include "comp/Transform.h"
#include "ds/Vector.h"
#include "gfx/Framebuffer.h"
#include "gfx/Image.h"
#include "gfx/Model.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"
#include "math/Vector.h"
#include "world/Object.h"
#include "world/Space.h"
#include "world/World.h"

namespace Gfx {
namespace Renderer {

Gfx::Shader nColorShader;
Gfx::Shader nFramebufferShader;
Gfx::Shader nMemberIdShader;

constexpr size_t nQuadVertexArraySize = 12;
GLuint nFullscreenVao;
GLuint nFullscreenVbo;
GLuint nSpriteVao;
GLuint nSpriteVbo;

Ds::Vector<unsigned int> nQueuedFullscreenFramebuffers;

void Init()
{
  // Initialize all of the expected shader types.
  Util::Result result =
    nColorShader.Init("vres/shader/Default.vs", "vres/shader/Color.fs");
  LogAbortIf(!result.Success(), result.mError.c_str());
  result = nFramebufferShader.Init(
    "vres/shader/Fullscreen.vs", "vres/shader/Fullscreen.fs");
  LogAbortIf(!result.Success(), result.mError.c_str());
  result =
    nMemberIdShader.Init("vres/shader/Default.vs", "vres/shader/MemberId.fs");
  LogAbortIf(!result.Success(), result.mError.c_str());

  // Initialize the fullscreen and sprite vertex arrays.
  // clang-format off
  float fullscreenVertices[nQuadVertexArraySize] =
    {-1.0f,  1.0f,
     -1.0f, -1.0f,
      1.0f,  1.0f,
      1.0f, -1.0f,
      1.0f,  1.0f,
     -1.0f, -1.0f};
  float spriteVertices[nQuadVertexArraySize] =
    {-0.5f,  0.5f,
     -0.5f, -0.5f,
      0.5f,  0.5f,
      0.5f, -0.5f,
      0.5f,  0.5f,
     -0.5f, -0.5f};
  // clang-format on
  auto uploadQuadVertexArray =
    [](float vertices[nQuadVertexArraySize], GLuint* vao, GLuint* vbo)
  {
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    GLuint size = sizeof(float) * nQuadVertexArraySize;
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
      0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  };
  uploadQuadVertexArray(fullscreenVertices, &nFullscreenVao, &nFullscreenVbo);
  uploadQuadVertexArray(spriteVertices, &nSpriteVao, &nSpriteVbo);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT, GL_FILL);
}

void Clear()
{
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Mat4 GetTransformation(const World::Space& space, World::MemberId memberId)
{
  Comp::Transform* transform = space.GetComponent<Comp::Transform>(memberId);
  if (transform == nullptr)
  {
    Mat4 identity;
    Math::Identity(&identity);
    return identity;
  }
  return transform->GetWorldMatrix(space, memberId);
}

Mat4 GetImageTransformation(
  const World::Space& space, World::MemberId memberId, const Gfx::Image& image)
{
  Mat4 transformation = GetTransformation(space, memberId);
  Mat4 aspectScale;
  Math::Scale(&aspectScale, {image.Aspect(), 1.0f, 1.0f});
  transformation *= aspectScale;
  return transformation;
}

void RenderQuad(GLuint vao)
{
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, nQuadVertexArraySize / 2);
  glBindVertexArray(0);
}

void RenderMemberIds(const World::Space& space, const Mat4& view)
{
  nMemberIdShader.Use();
  nMemberIdShader.SetMat4("uView", view.CData());
  nMemberIdShader.SetMat4("uProj", Viewport::Perspective().CData());

  // Render MemberIds for every model.
  space.VisitTableComponents<Comp::Model>(
    [&space](World::MemberId owner, const Comp::Model& modelComp)
    {
      Mat4 transformation = GetTransformation(space, owner);
      nMemberIdShader.Use();
      nMemberIdShader.SetMat4("uModel", transformation.CData());
      nMemberIdShader.SetInt("uMemberId", owner);
      Gfx::Model& model = AssLib::Get<Gfx::Model>(modelComp.mModelId);
      model.Draw(nMemberIdShader);
    });

  // Render MemberIds for every sprite.
  glDisable(GL_CULL_FACE);
  space.VisitTableComponents<Comp::Sprite>(
    [&space](World::MemberId owner, const Comp::Sprite& spriteComp)
    {
      const Gfx::Image& image = AssLib::Get<Gfx::Image>(spriteComp.mImageId);
      Mat4 transformation = GetImageTransformation(space, owner, image);
      nMemberIdShader.Use();
      nMemberIdShader.SetMat4("uModel", transformation.CData());
      nMemberIdShader.SetInt("uMemberId", owner);
      RenderQuad(nSpriteVao);
    });
  glEnable(GL_CULL_FACE);
}

World::MemberId HoveredMemberId(const World::Space& space, const Mat4& view)
{
  // Render all of the MemberIds to a framebuffer.
  Gfx::Framebuffer handlebuffer(GL_RED_INTEGER, GL_INT);
  glBindFramebuffer(GL_FRAMEBUFFER, handlebuffer.Fbo());
  glClearBufferiv(GL_COLOR, 0, &World::nInvalidMemberId);
  glClear(GL_DEPTH_BUFFER_BIT);
  Gfx::Renderer::RenderMemberIds(space, view);

  // Find the MemberId at the mouse position.
  World::MemberId memberId;
  const Vec2& mousePos = Input::MousePosition();
  glReadPixels(
    (int)mousePos[0],
    Viewport::Height() - (int)mousePos[1],
    1,
    1,
    handlebuffer.Format(),
    handlebuffer.PixelType(),
    (void*)&memberId);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return memberId;
}

void RenderSpace(const World::Space& space)
{
  // Find the view matrix using the space's camera and render the space.
  Comp::Transform* cameraTransform = nullptr;
  if (space.mCameraId != World::nInvalidMemberId)
  {
    cameraTransform = space.GetComponent<Comp::Transform>(space.mCameraId);
  }
  Mat4 view;
  Vec3 viewPos;
  if (cameraTransform == nullptr)
  {
    Math::Identity(&view);
    viewPos = {0.0, 0.0f, 0.0f};
  } else
  {
    view = cameraTransform->GetInverseWorldMatrix(space, space.mCameraId);
    viewPos = cameraTransform->GetWorldTranslation(space, space.mCameraId);
  }
  RenderSpace(space, view, viewPos);
}

void RenderSpace(
  const World::Space& space, const Mat4& view, const Vec3& viewPos)
{
  // Render all of the Model components.
  space.VisitTableComponents<Comp::Model>(
    [&](World::MemberId owner, const Comp::Model& modelComp)
    {
      const Gfx::Shader& shader = AssLib::Get<Gfx::Shader>(modelComp.mShaderId);
      Mat4 transformation = GetTransformation(space, owner);
      shader.Use();
      shader.SetMat4("uModel", transformation.CData());
      shader.SetMat4("uView", view.CData());
      shader.SetMat4("uProj", Viewport::Perspective().CData());
      shader.SetVec3("uViewPos", viewPos.CData());
      shader.SetFloat("uTime", Framer::TotalTime());
      const Gfx::Model& model = AssLib::Get<Gfx::Model>(modelComp.mModelId);
      model.Draw(shader);
    });

  // Render all of the Sprite components.
  glDisable(GL_CULL_FACE);
  space.VisitTableComponents<Comp::Sprite>(
    [&](World::MemberId owner, const Comp::Sprite& spriteComp)
    {
      const Gfx::Shader& shader =
        AssLib::Get<Gfx::Shader>(spriteComp.mShaderId);
      const Gfx::Image& image = AssLib::Get<Gfx::Image>(spriteComp.mImageId);
      Mat4 transformation = GetImageTransformation(space, owner, image);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, image.Id());
      shader.Use();
      shader.SetMat4("uModel", transformation.CData());
      shader.SetMat4("uView", view.CData());
      shader.SetMat4("uProj", Viewport::Perspective().CData());
      shader.SetInt("uTexture", 0);
      RenderQuad(nSpriteVao);
      glBindTexture(GL_TEXTURE_2D, 0);
    });
  glEnable(GL_CULL_FACE);
}

void RenderWorld()
{
  World::SpaceVisitor visitor;
  while (!visitor.End())
  {
    RenderSpace(visitor.CurrentSpace());
    visitor.Next();
  }
}

void RenderQueuedFullscreenFramebuffers()
{
  for (unsigned int tbo : nQueuedFullscreenFramebuffers)
  {
    glDisable(GL_DEPTH_TEST);
    nFramebufferShader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tbo);
    nFramebufferShader.SetInt("uTexture", 0);
    RenderQuad(nFullscreenVao);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);
  }
  nQueuedFullscreenFramebuffers.Clear();
}

void QueueFullscreenFramebuffer(const Framebuffer& framebuffer)
{
  nQueuedFullscreenFramebuffers.Push(framebuffer.ColorTbo());
}

const Shader& ColorShader()
{
  return nColorShader;
}

} // namespace Renderer
} // namespace Gfx
