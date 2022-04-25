#include <glad/glad.h>
#include <sstream>

#include "AssetLibrary.h"
#include "Input.h"
#include "Temporal.h"
#include "Viewport.h"
#include "comp/AlphaColor.h"
#include "comp/Camera.h"
#include "comp/DirectionalLight.h"
#include "comp/Model.h"
#include "comp/Sprite.h"
#include "comp/Text.h"
#include "comp/Transform.h"
#include "debug/Draw.h"
#include "ds/Vector.h"
#include "editor/Editor.h"
#include "editor/OverviewInterface.h"
#include "gfx/Framebuffer.h"
#include "gfx/Image.h"
#include "gfx/Model.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"
#include "math/Vector.h"
#include "world/Object.h"
#include "world/Space.h"
#include "world/World.h"

namespace Gfx::Renderer {

constexpr size_t nQuadVertexArraySize = 12;
GLuint nFullscreenVao;
GLuint nFullscreenVbo;
GLuint nSpriteVao;
GLuint nSpriteVbo;

GLuint nMatricesUniformBufferVbo;
GLuint nLightsUniformBufferVbo;

size_t nNextSpaceFramebuffer;
Ds::Vector<Framebuffer> nSpaceFramebuffers;

void (*nCustomRender)();

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

  auto CreateUniformBuffer = [](GLuint* vbo, GLsizeiptr size, GLuint binding)
  {
    glGenBuffers(1, vbo);
    glBindBuffer(GL_UNIFORM_BUFFER, *vbo);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, *vbo);
  };
  CreateUniformBuffer(&nMatricesUniformBufferVbo, 128, 0);
  CreateUniformBuffer(&nLightsUniformBufferVbo, 64, 1);

  nNextSpaceFramebuffer = 0;

  nCustomRender = nullptr;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Purge()
{
  nSpaceFramebuffers.Clear();
}

void Clear()
{
  // Remove any space framebuffers that were not used during the last render and
  // clear those that were used.
  for (size_t i = nNextSpaceFramebuffer; i < nSpaceFramebuffers.Size(); ++i) {
    nSpaceFramebuffers.Pop();
  }
  for (size_t i = 0; i < nSpaceFramebuffers.Size(); ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, nSpaceFramebuffers[i].Fbo());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  nNextSpaceFramebuffer = 0;

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render()
{
  Clear();
  if (nCustomRender != nullptr) {
    nCustomRender();
  }
  if (Editor::nEditorMode) {
    // Render only the selected space and the editor space.
    Editor::OverviewInterface* overviewInterface =
      Editor::nCoreInterface.FindInterface<Editor::OverviewInterface>();
    if (overviewInterface == nullptr) {
      return;
    }
    const Mat4& view = Editor::nCamera.View();
    const Mat4& proj = Editor::nCamera.Proj();
    const Vec3& position = Editor::nCamera.Position();
    RenderSpace(*overviewInterface->mSpace, view, proj, position);
    RenderSpace(Editor::nSpace, view, proj, position);
    RenderFramebuffers();
    Debug::Draw::Render(Editor::nCamera.View(), Editor::nCamera.Proj());
  }
  else {
    Result result = RenderWorld();
    if (!result.Success()) {
      LogError(result.mError.c_str());
      Editor::nEditorMode = true;
    }
    RenderFramebuffers();
  }
}

Mat4 GetTransformation(const World::Object& object)
{
  Comp::Transform* transform = object.TryGetComponent<Comp::Transform>();
  if (transform == nullptr) {
    Mat4 identity;
    Math::Identity(&identity);
    return identity;
  }
  return transform->GetWorldMatrix(object);
}

Mat4 GetImageTransformation(
  const World::Object& object, const Gfx::Image& image)
{
  Mat4 transformation = GetTransformation(object);
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

void InitializeMatricesUniformBuffer(const Mat4& view, const Mat4& proj)
{
  Mat4 viewTranspose = Math::Transpose(view);
  Mat4 projTranspose = Math::Transpose(proj);
  glBindBuffer(GL_UNIFORM_BUFFER, nMatricesUniformBufferVbo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Mat4), viewTranspose.CData());
  glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(Mat4), projTranspose.CData());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void InitializeLightsUniformBuffer(const World::Space& space)
{
  GLenum buffer = GL_UNIFORM_BUFFER;
  glBindBuffer(buffer, nLightsUniformBufferVbo);
  space.VisitTableComponents<Comp::DirectionalLight>(
    [&](World::MemberId owner, const Comp::DirectionalLight& light)
    {
      glBufferSubData(buffer, 0, sizeof(Vec3), light.mDirection.CData());
      glBufferSubData(buffer, 16, sizeof(Vec3), light.mAmbient.CData());
      glBufferSubData(buffer, 32, sizeof(Vec3), light.mDiffuse.CData());
      glBufferSubData(buffer, 48, sizeof(Vec3), light.mSpecular.CData());
    });
  glBindBuffer(buffer, 0);
}

void RenderMemberIds(
  const World::Space& space, const Mat4& view, const Mat4& proj)
{
  const Gfx::Shader* memberIdShader =
    AssLib::TryGetLive<Gfx::Shader>(AssLib::nMemberIdShaderId);
  if (memberIdShader == nullptr) {
    return;
  }
  InitializeMatricesUniformBuffer(view, proj);

  GLint modelLoc = memberIdShader->UniformLocation(Uniform::Type::Model);
  GLint memberIdLoc = memberIdShader->UniformLocation(Uniform::Type::MemberId);

  glUseProgram(memberIdShader->Id());
  // Render MemberIds for every model.
  space.VisitTableComponents<Comp::Model>(
    [&](World::MemberId owner, const Comp::Model& modelComp)
    {
      const Gfx::Model* model =
        AssLib::TryGetLive<Gfx::Model>(modelComp.mModelId);
      if (model == nullptr) {
        return;
      }

      glUniform1i(memberIdLoc, (int)owner);
      World::Object object(const_cast<World::Space*>(&space), owner);
      Mat4 memberTransformation = GetTransformation(object);
      for (const Gfx::Model::DrawInfo& drawInfo : model->GetAllDrawInfo()) {
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
      if (image == nullptr) {
        return;
      }

      World::Object object(const_cast<World::Space*>(&space), owner);
      Mat4 transformation = GetImageTransformation(object, *image);
      glUniform1i(memberIdLoc, (int)owner);
      glUniformMatrix4fv(modelLoc, 1, true, transformation.CData());
      RenderQuad(nSpriteVao);
    });

  // Render MemberIds for every text component.
  space.VisitTableComponents<Comp::Text>(
    [&](World::MemberId owner, const Comp::Text& textComp)
    {
      World::Object object(const_cast<World::Space*>(&space), owner);
      Mat4 baseTransformation = GetTransformation(object);
      Ds::Vector<Comp::Text::DrawInfo> allDrawInfo = textComp.GetAllDrawInfo();
      for (const Comp::Text::DrawInfo& drawInfo : allDrawInfo) {
        Mat4 glyphTransformation = baseTransformation * drawInfo.mOffset;
        glUniform1i(memberIdLoc, (int)owner);
        glUniformMatrix4fv(modelLoc, 1, true, glyphTransformation.CData());
        RenderQuad(nSpriteVao);
      }
    });
  glEnable(GL_CULL_FACE);
}

World::MemberId HoveredMemberId(
  const World::Space& space, const Mat4& view, const Mat4& proj)
{
  // Render all of the MemberIds to a framebuffer.
  Gfx::Framebuffer handlebuffer(GL_RED_INTEGER, GL_INT);
  glBindFramebuffer(GL_FRAMEBUFFER, handlebuffer.Fbo());
  glClearBufferiv(GL_COLOR, 0, &World::nInvalidMemberId);
  glClear(GL_DEPTH_BUFFER_BIT);
  Gfx::Renderer::RenderMemberIds(space, view, proj);

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

Result RenderSpace(const World::Space& space)
{
  // Make sure the space has a camera.
  if (space.mCameraId == World::nInvalidMemberId) {
    std::stringstream error;
    error << "Space \"" << space.mName << "\" has no assigned camera.";
    return Result(error.str());
  }
  // Make sure the camera has a camera component.
  const Comp::Camera* cameraComp =
    space.TryGetComponent<Comp::Camera>(space.mCameraId);
  if (cameraComp == nullptr) {
    std::stringstream error;
    error << "Space \"" << space.mName
          << "\"'s camera does not have a camera component";
    return Result(error.str());
  }

  // Find the view matrix using the space's camera and render the space.
  Comp::Transform& transformComp =
    space.GetComponent<Comp::Transform>(space.mCameraId);
  World::Object object(const_cast<World::Space*>(&space), space.mCameraId);
  Mat4 view = transformComp.GetInverseWorldMatrix(object);
  Mat4 proj = cameraComp->Proj();
  Vec3 viewPos = transformComp.GetWorldTranslation(object);
  RenderSpace(space, view, proj, viewPos);
  return Result();
}

void RenderSpace(
  const World::Space& space,
  const Mat4& view,
  const Mat4& proj,
  const Vec3& viewPos)
{
  InitializeMatricesUniformBuffer(view, proj);
  InitializeLightsUniformBuffer(space);

  // Get the next space framebuffer that hasn't been rendered to and bind it.
  if (nNextSpaceFramebuffer >= nSpaceFramebuffers.Size()) {
    nSpaceFramebuffers.Emplace(GL_RGBA, GL_UNSIGNED_BYTE);
  }
  const Framebuffer& framebuffer = nSpaceFramebuffers[nNextSpaceFramebuffer];
  ++nNextSpaceFramebuffer;
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.Fbo());

  // Render all of the Model components.
  space.VisitTableComponents<Comp::Model>(
    [&](World::MemberId owner, const Comp::Model& modelComp)
    {
      if (!modelComp.mVisible) {
        return;
      }
      const Gfx::Shader* shader =
        AssLib::TryGetLive<Gfx::Shader>(modelComp.mShaderId);
      const Gfx::Model* model =
        AssLib::TryGetLive<Gfx::Model>(modelComp.mModelId);
      if (shader == nullptr || model == nullptr) {
        return;
      }

      GLint modelLoc = shader->UniformLocation(Uniform::Type::Model);
      GLint viewPosLoc = shader->UniformLocation(Uniform::Type::ViewPos);
      GLint timeLoc = shader->UniformLocation(Uniform::Type::Time);
      GLint diffuseLoc = shader->UniformLocation(Uniform::Type::ADiffuse);
      GLint specLoc = shader->UniformLocation(Uniform::Type::ASpecular);

      glUseProgram(shader->Id());
      glUniform3fv(viewPosLoc, 1, viewPos.CData());
      glUniform1f(timeLoc, Temporal::TotalTime());
      Comp::AlphaColor* alphaColorComp =
        space.TryGetComponent<Comp::AlphaColor>(owner);
      if (alphaColorComp != nullptr) {
        GLint alphaColorLoc =
          shader->UniformLocation(Uniform::Type::AlphaColor);
        glUniform4fv(alphaColorLoc, 1, alphaColorComp->mAlphaColor.CData());
      }

      World::Object object(const_cast<World::Space*>(&space), owner);
      Mat4 memberTransformation = GetTransformation(object);
      for (const Gfx::Model::DrawInfo& drawInfo : model->GetAllDrawInfo()) {
        // Prepare all of the textures.
        const Material& material = model->GetMaterial(drawInfo.mMaterialIndex);
        int textureCounter = 0;
        Ds::Vector<GLint> indices;
        for (const Material::TextureGroup& group : material.mGroups) {
          indices.Reserve(group.mImages.Size());
          for (const Image& image : group.mImages) {
            glActiveTexture(GL_TEXTURE0 + textureCounter);
            glBindTexture(GL_TEXTURE_2D, image.Id());
            indices.Push(textureCounter);
            ++textureCounter;
          }
          switch (group.mType) {
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
      if (shader == nullptr || image == nullptr) {
        return;
      }

      GLint modelLoc = shader->UniformLocation(Uniform::Type::Model);
      GLint samplerLoc = shader->UniformLocation(Uniform::Type::Sampler);

      glUseProgram(shader->Id());
      glUniform1i(samplerLoc, 0);
      World::Object object(const_cast<World::Space*>(&space), owner);
      Mat4 transformation = GetImageTransformation(object, *image);
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
      if (!textComp.mVisible) {
        return;
      }
      const Gfx::Shader* shader =
        AssLib::TryGetLive<Gfx::Shader>(textComp.mShaderId);
      if (shader == nullptr) {
        shader = AssLib::TryGetLive<Gfx::Shader>(AssLib::nDefaultTextShaderId);
      }

      GLint modelLoc = shader->UniformLocation(Uniform::Type::Model);
      GLint colorLoc = shader->UniformLocation(Uniform::Type::Color);
      GLint samplerLoc = shader->UniformLocation(Uniform::Type::Sampler);
      GLint fillAmountLoc = shader->UniformLocation(Uniform::Type::FillAmount);

      glUseProgram(shader->Id());
      glUniform1i(samplerLoc, 0);
      glUniform1f(fillAmountLoc, textComp.mFillAmount);

      Comp::AlphaColor* alphaColorComp =
        space.TryGetComponent<Comp::AlphaColor>(owner);
      if (alphaColorComp != nullptr) {
        glUniform4fv(colorLoc, 1, alphaColorComp->mAlphaColor.CData());
      }
      else {
        Vec4 defaultColor = {0.0f, 1.0f, 0.0f, 1.0f};
        glUniform4fv(colorLoc, 1, defaultColor.CData());
      }

      World::Object object(const_cast<World::Space*>(&space), owner);
      Mat4 baseTransformation = GetTransformation(object);
      Ds::Vector<Comp::Text::DrawInfo> allDrawInfo = textComp.GetAllDrawInfo();
      for (const Comp::Text::DrawInfo& drawInfo : allDrawInfo) {
        Mat4 glyphTransformation = baseTransformation * drawInfo.mOffset;
        glUniformMatrix4fv(modelLoc, 1, true, glyphTransformation.CData());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, drawInfo.mId);
        RenderQuad(nSpriteVao);
      }
    });
  glEnable(GL_CULL_FACE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Result RenderWorld()
{
  for (const World::Space& space : World::nSpaces) {
    Result result = RenderSpace(space);
    if (!result.Success()) {
      return result;
    }
  }
  return Result();
}

void RenderFramebuffers()
{
  const Gfx::Shader* shader =
    AssLib::TryGetLive<Gfx::Shader>(AssLib::nFramebufferShaderId);
  if (shader == nullptr) {
    return;
  }

  GLint samplerLoc = shader->UniformLocation(Uniform::Type::Sampler);
  glUseProgram(shader->Id());
  glUniform1i(samplerLoc, 0);

  glDisable(GL_DEPTH_TEST);
  for (size_t i = 0; i < nNextSpaceFramebuffer; ++i) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nSpaceFramebuffers[i].ColorTbo());
    RenderQuad(nFullscreenVao);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_DEPTH_TEST);
}

} // namespace Gfx::Renderer
