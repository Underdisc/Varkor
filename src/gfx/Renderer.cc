#include <glad/glad.h>

#include "AssetLibrary.h"
#include "Input.h"
#include "Temporal.h"
#include "Viewport.h"
#include "comp/Model.h"
#include "comp/Sprite.h"
#include "comp/Text.h"
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

constexpr size_t nQuadVertexArraySize = 12;
GLuint nFullscreenVao;
GLuint nFullscreenVbo;
GLuint nSpriteVao;
GLuint nSpriteVbo;

Ds::Vector<unsigned int> nQueuedFullscreenFramebuffers;

void Init()
{
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
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
  const Gfx::Shader* memberIdShader =
    AssLib::TryGetLive<Gfx::Shader>(AssLib::nMemberIdShaderId);
  if (memberIdShader == nullptr)
  {
    return;
  }

  GLint viewLoc = memberIdShader->UniformLocation(Uniform::Type::View);
  GLint projLoc = memberIdShader->UniformLocation(Uniform::Type::Proj);
  GLint modelLoc = memberIdShader->UniformLocation(Uniform::Type::Model);
  GLint memberIdLoc = memberIdShader->UniformLocation(Uniform::Type::MemberId);

  glUseProgram(memberIdShader->Id());
  glUniformMatrix4fv(viewLoc, 1, true, view.CData());
  glUniformMatrix4fv(projLoc, 1, true, Viewport::Perspective().CData());

  // Render MemberIds for every model.
  space.VisitTableComponents<Comp::Model>(
    [&](World::MemberId owner, const Comp::Model& modelComp)
    {
      const Gfx::Model* model =
        AssLib::TryGetLive<Gfx::Model>(modelComp.mModelId);
      if (model == nullptr)
      {
        return;
      }

      glUniform1i(memberIdLoc, (int)owner);
      Mat4 memberTransformation = GetTransformation(space, owner);
      for (const Gfx::Model::DrawInfo& drawInfo : model->GetAllDrawInfo())
      {
        Mat4 transformation = memberTransformation * drawInfo.mTransformation;
        glUniformMatrix4fv(modelLoc, 1, true, transformation.CData());
        const Gfx::Mesh& mesh = model->GetMesh(drawInfo.mMeshIndex);
        glBindVertexArray(mesh.Vao());
        glDrawElements(
          GL_TRIANGLES, (GLsizei)mesh.IndexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
      }
    });

  // Render MemberIds for every sprite.
  glDisable(GL_CULL_FACE);
  space.VisitTableComponents<Comp::Sprite>(
    [&](World::MemberId owner, const Comp::Sprite& spriteComp)
    {
      const Gfx::Image* image =
        AssLib::TryGetLive<Gfx::Image>(spriteComp.mImageId);
      if (image == nullptr)
      {
        return;
      }

      Mat4 transformation = GetImageTransformation(space, owner, *image);
      glUniform1i(memberIdLoc, (int)owner);
      glUniformMatrix4fv(modelLoc, 1, true, transformation.CData());
      RenderQuad(nSpriteVao);
    });

  // Render MemberIds for every text component.
  space.VisitTableComponents<Comp::Text>(
    [&](World::MemberId owner, const Comp::Text& textComp)
    {
      Mat4 baseTransformation = GetTransformation(space, owner);
      Ds::Vector<Comp::Text::DrawInfo> allDrawInfo = textComp.GetAllDrawInfo();
      for (const Comp::Text::DrawInfo& drawInfo : allDrawInfo)
      {
        Mat4 glyphTransformation = baseTransformation * drawInfo.mOffset;
        glUniform1i(memberIdLoc, (int)owner);
        glUniformMatrix4fv(modelLoc, 1, true, glyphTransformation.CData());
        RenderQuad(nSpriteVao);
      }
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
      const Gfx::Shader* shader =
        AssLib::TryGetLive<Gfx::Shader>(modelComp.mShaderId);
      const Gfx::Model* model =
        AssLib::TryGetLive<Gfx::Model>(modelComp.mModelId);
      if (shader == nullptr || model == nullptr)
      {
        return;
      }

      GLint viewLoc = shader->UniformLocation(Uniform::Type::View);
      GLint projLoc = shader->UniformLocation(Uniform::Type::Proj);
      GLint modelLoc = shader->UniformLocation(Uniform::Type::Model);
      GLint viewPosLoc = shader->UniformLocation(Uniform::Type::ViewPos);
      GLint timeLoc = shader->UniformLocation(Uniform::Type::Time);
      GLint diffuseLoc = shader->UniformLocation(Uniform::Type::ADiffuse);
      GLint specLoc = shader->UniformLocation(Uniform::Type::ASpecular);

      glUseProgram(shader->Id());
      glUniformMatrix4fv(viewLoc, 1, true, view.CData());
      glUniformMatrix4fv(projLoc, 1, true, Viewport::Perspective().CData());
      glUniform3fv(viewPosLoc, 1, viewPos.CData());
      glUniform1f(timeLoc, Temporal::TotalTime());

      Mat4 memberTransformation = GetTransformation(space, owner);
      for (const Gfx::Model::DrawInfo& drawInfo : model->GetAllDrawInfo())
      {
        // Prepare all of the textures.
        const Material& material = model->GetMaterial(drawInfo.mMaterialIndex);
        int textureCounter = 0;
        Ds::Vector<GLint> indices;
        for (const Material::TextureGroup& group : material.mGroups)
        {
          indices.Reserve(group.mImages.Size());
          for (const Image& image : group.mImages)
          {
            glActiveTexture(GL_TEXTURE0 + textureCounter);
            glBindTexture(GL_TEXTURE_2D, image.Id());
            indices.Push(textureCounter);
            ++textureCounter;
          }
          switch (group.mType)
          {
          case Material::TextureType::Diffuse:
            glUniform1iv(diffuseLoc, (GLsizei)indices.Size(), indices.CData());
            break;
          case Material::TextureType::Specular:
            glUniform1iv(specLoc, (GLsizei)indices.Size(), indices.CData());
            break;
          }
          indices.Clear();
        }

        // Render the mesh.
        Mat4 transformation = memberTransformation * drawInfo.mTransformation;
        glUniformMatrix4fv(modelLoc, 1, true, transformation.CData());
        const Mesh& mesh = model->GetMesh(drawInfo.mMeshIndex);
        glBindVertexArray(mesh.Vao());
        glDrawElements(
          GL_TRIANGLES, (GLsizei)mesh.IndexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
      }
    });

  glDisable(GL_CULL_FACE);
  // Render all of the Sprite components.
  space.VisitTableComponents<Comp::Sprite>(
    [&](World::MemberId owner, const Comp::Sprite& spriteComp)
    {
      const Gfx::Shader* shader =
        AssLib::TryGetLive<Gfx::Shader>(spriteComp.mShaderId);
      const Gfx::Image* image =
        AssLib::TryGetLive<Gfx::Image>(spriteComp.mImageId);
      if (shader == nullptr || image == nullptr)
      {
        return;
      }

      GLint projLoc = shader->UniformLocation(Uniform::Type::Proj);
      GLint viewLoc = shader->UniformLocation(Uniform::Type::View);
      GLint modelLoc = shader->UniformLocation(Uniform::Type::Model);
      GLint samplerLoc = shader->UniformLocation(Uniform::Type::Sampler);

      glUseProgram(shader->Id());
      glUniformMatrix4fv(projLoc, 1, true, Viewport::Perspective().CData());
      glUniformMatrix4fv(viewLoc, 1, true, view.CData());
      glUniform1i(samplerLoc, 0);

      Mat4 transformation = GetImageTransformation(space, owner, *image);
      glUniformMatrix4fv(modelLoc, 1, true, transformation.CData());

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, image->Id());
      RenderQuad(nSpriteVao);
      glBindTexture(GL_TEXTURE_2D, 0);
    });

  // Render all of the Text components.
  space.VisitTableComponents<Comp::Text>(
    [&](World::MemberId owner, Comp::Text& textComp)
    {
      const Gfx::Shader* shader =
        AssLib::TryGetLive<Gfx::Shader>(textComp.mShaderId);
      if (shader == nullptr)
      {
        shader = AssLib::TryGetLive<Gfx::Shader>(AssLib::nDefaultTextShaderId);
      }

      GLint projLoc = shader->UniformLocation(Uniform::Type::Proj);
      GLint viewLoc = shader->UniformLocation(Uniform::Type::View);
      GLint modelLoc = shader->UniformLocation(Uniform::Type::Model);
      GLint colorLoc = shader->UniformLocation(Uniform::Type::Color);
      GLint samplerLoc = shader->UniformLocation(Uniform::Type::Sampler);

      glUseProgram(shader->Id());
      glUniformMatrix4fv(projLoc, 1, true, Viewport::Perspective().CData());
      glUniformMatrix4fv(viewLoc, 1, true, view.CData());
      glUniform1i(samplerLoc, 0);
      glUniform3fv(colorLoc, 1, textComp.mColor.CData());

      Mat4 baseTransformation = GetTransformation(space, owner);
      Ds::Vector<Comp::Text::DrawInfo> allDrawInfo = textComp.GetAllDrawInfo();
      for (const Comp::Text::DrawInfo& drawInfo : allDrawInfo)
      {
        Mat4 glyphTransformation = baseTransformation * drawInfo.mOffset;
        glUniformMatrix4fv(modelLoc, 1, true, glyphTransformation.CData());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, drawInfo.mId);
        RenderQuad(nSpriteVao);
      }
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
  const Gfx::Shader* shader =
    AssLib::TryGetLive<Gfx::Shader>(AssLib::nFramebufferShaderId);
  if (shader == nullptr)
  {
    return;
  }

  GLint samplerLoc = shader->UniformLocation(Uniform::Type::Sampler);
  glUseProgram(shader->Id());
  glUniform1i(samplerLoc, 0);

  for (unsigned int tbo : nQueuedFullscreenFramebuffers)
  {
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tbo);
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

} // namespace Renderer
} // namespace Gfx
