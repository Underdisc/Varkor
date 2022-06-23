#include <glad/glad.h>

#include "comp/DefaultPostProcess.h"
#include "editor/AssetInterfaces.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"

AssetId DefaultPostProcess::smDefaultShaderId;

void DefaultPostProcess::VStaticInit()
{
  smDefaultShaderId = AssLib::Create<Gfx::Shader>(
    "DefaultPostProcess",
    "vres/shader/FullscreenTexture.glv",
    "vres/shader/DefaultPostProcess.glf");
}

void DefaultPostProcess::VInit(const World::Object& owner)
{
  mShaderId = smDefaultShaderId;
}

void DefaultPostProcess::VSerialize(Vlk::Value& val)
{
  val("ShaderId") = mShaderId;
}

void DefaultPostProcess::VDeserialize(const Vlk::Explorer& ex)
{
  mShaderId = ex("ShaderId").As<AssetId>(smDefaultShaderId);
}

void DefaultPostProcess::VRender(const World::Object& owner)
{
  const Gfx::Shader* shader = AssLib::TryGetLive<Gfx::Shader>(mShaderId);
  if (shader == nullptr) {
    return;
  }

  GLint samplerLoc = shader->UniformLocation(Gfx::Uniform::Type::Sampler);
  glUseProgram(shader->Id());
  glUniform1i(samplerLoc, 0);
  Gfx::Renderer::RenderCurrentSpaceFramebuffer();
}

void DefaultPostProcess::VEdit(const World::Object& owner)
{
  Editor::DropAssetWidget<Gfx::Shader>(&mShaderId);
}