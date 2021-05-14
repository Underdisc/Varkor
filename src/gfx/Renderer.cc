#include <glad/glad.h>

#include "AssetLibrary.h"
#include "Input.h"
#include "Viewport.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "ds/Vector.h"
#include "gfx/Framebuffer.h"
#include "math/Vector.h"
#include "world/Object.h"
#include "world/Space.h"
#include "world/World.h"

#include "gfx/Renderer.h"

namespace Gfx {
namespace Renderer {

Shader nColorShader;
Shader nDefaultShader;
Shader nFramebufferShader;
Shader nMemberIdShader;

constexpr unsigned int nFullscreenVertexCount = 12;
unsigned int nFullscreenVao;
unsigned int nFullscreenVbo;
Ds::Vector<unsigned int> nQueuedFullscreenFramebuffers;

void Init()
{
  // Initialize all of the expected shader types.
  Shader::InitResult result =
    nColorShader.Init("vres/shader/Default.vs", "vres/shader/Color.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());
  result =
    nDefaultShader.Init("vres/shader/Default.vs", "vres/shader/Default.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());
  result = nFramebufferShader.Init(
    "vres/shader/Fullscreen.vs", "vres/shader/Fullscreen.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());
  result =
    nMemberIdShader.Init("vres/shader/Default.vs", "vres/shader/MemberId.fs");
  LogAbortIf(!result.mSuccess, result.mError.c_str());

  // Initialize the fullscreen quad vertex array that will be used for drawing
  // fullscreen framebuffers.
  // clang-format off
  float fullscreenVerts[nFullscreenVertexCount] =
    {-1.0f,  1.0f,
     -1.0f, -1.0f,
      1.0f,  1.0f,
      1.0f, -1.0f,
      1.0f,  1.0f,
     -1.0f, -1.0f};
  // clang-format on
  glGenVertexArrays(1, &nFullscreenVao);
  glBindVertexArray(nFullscreenVao);
  glGenBuffers(1, &nFullscreenVbo);
  glBindBuffer(GL_ARRAY_BUFFER, nFullscreenVbo);
  unsigned int size = sizeof(float) * nFullscreenVertexCount;
  glBufferData(GL_ARRAY_BUFFER, size, fullscreenVerts, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT, GL_FILL);
}

void Clear()
{
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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

void RenderModel(const Shader& shader, const Comp::Model& modelComp)
{
  const AssetLibrary::ModelAsset* modelAsset =
    AssetLibrary::GetModel(modelComp.mModelId);
  if (modelAsset != nullptr)
  {
    modelAsset->mModel.Draw(shader);
  }
}

void RenderMemberIds(const World::Space& space, const Mat4& view)
{
  nMemberIdShader.Use();
  nMemberIdShader.SetMat4("uView", view.CData());
  nMemberIdShader.SetMat4("uProj", Viewport::Perspective().CData());

  // Render every model in the space and use the model owner's MemberId as the
  // color value.
  World::Table::Visitor<Comp::Model> visitor =
    space.CreateTableVisitor<Comp::Model>();
  while (!visitor.End())
  {
    Mat4 model = GetTransformation(space, visitor.CurrentOwner());
    nMemberIdShader.SetMat4("uModel", model.CData());
    nMemberIdShader.SetInt("uMemberId", visitor.CurrentOwner());
    const Comp::Model& modelComp = visitor.CurrentComponent();
    RenderModel(nMemberIdShader, modelComp);
    visitor.Next();
  }
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

void RenderModels(const World::Space& space, const Mat4& view)
{
  // Visit all of the model components within the space.
  World::Table::Visitor<Comp::Model> visitor =
    space.CreateTableVisitor<Comp::Model>();
  while (!visitor.End())
  {
    // Find the shader that will be used to draw the model.
    const Comp::Model& modelComp = visitor.CurrentComponent();
    const AssetLibrary::ShaderAsset* shaderAsset =
      AssetLibrary::GetShader(modelComp.mShaderId);
    const Shader* drawShader;
    if (shaderAsset != nullptr && shaderAsset->mShader.Live())
    {
      drawShader = &shaderAsset->mShader;
    } else
    {
      drawShader = &nDefaultShader;
    }

    // Draw the model.
    drawShader->Use();
    Mat4 model = GetTransformation(space, visitor.CurrentOwner());
    drawShader->SetMat4("uModel", model.CData());
    drawShader->SetMat4("uView", view.CData());
    drawShader->SetMat4("uProj", Viewport::Perspective().CData());
    RenderModel(*drawShader, modelComp);
    visitor.Next();
  }
}

void RenderSpace(const World::Space& space, const Mat4& view)
{
  RenderModels(space, view);
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
  if (cameraTransform == nullptr)
  {
    Math::Identity(&view);
  } else
  {
    view = cameraTransform->GetWorldMatrix(space, space.mCameraId);
    view = Math::Inverse(view);
  }
  RenderModels(space, view);
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

void RenderFullscreenTexture(unsigned int tbo)
{
  glDisable(GL_DEPTH_TEST);
  nFramebufferShader.Use();
  glBindVertexArray(nFullscreenVao);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tbo);
  nFramebufferShader.SetInt("uTexture", 0);
  glDrawArrays(GL_TRIANGLES, 0, nFullscreenVertexCount / 2);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glEnable(GL_DEPTH_TEST);
}

void RenderQueuedFullscreenFramebuffers()
{
  for (unsigned int tbo : nQueuedFullscreenFramebuffers)
  {
    RenderFullscreenTexture(tbo);
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
