#include "comp/Skybox.h"
#include "gfx/Cubemap.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"

namespace Comp {

void Skybox::VInit(const World::Object& owner)
{
  mCubemapId = AssLib::nDefaultAssetId;
  mShaderId = AssLib::nDefaultSkyboxShaderId;
}

void Skybox::VSerialize(Vlk::Value& val)
{
  val("CubemapId") = mCubemapId;
  val("ShaderId") = mShaderId;
}

void Skybox::VDeserialize(const Vlk::Explorer& ex)
{
  mCubemapId = ex("CubemapId").As<AssetId>(AssLib::nDefaultAssetId);
  mShaderId = ex("ShaderId").As<AssetId>(AssLib::nDefaultSkyboxShaderId);
}

void Skybox::VRender(const World::Object& owner)
{
  const Gfx::Cubemap* cubemap = AssLib::TryGetLive<Gfx::Cubemap>(mCubemapId);
  const Gfx::Shader* shader = AssLib::TryGetLive<Gfx::Shader>(mShaderId);
  if (cubemap == nullptr || shader == nullptr) {
    return;
  }

  GLint skyboxSamplerLoc =
    shader->UniformLocation(Gfx::Uniform::Type::SkyboxSampler);
  glUseProgram(shader->Id());
  glUniform1i(skyboxSamplerLoc, 0);

  glBindVertexArray(Gfx::Renderer::nSkyboxVao);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->Id());
  glDepthMask(GL_FALSE);
  glDrawElements(
    GL_TRIANGLES, Gfx::Renderer::nSkyboxElementCount, GL_UNSIGNED_INT, 0);
  glDepthMask(GL_TRUE);
  glBindVertexArray(0);
}

} // namespace Comp

namespace Editor {

void Hook<Comp::Skybox>::Edit(const World::Object& object)
{
  auto& skybox = object.Get<Comp::Skybox>();
  SelectAssetWidget<Gfx::Cubemap>(&skybox.mCubemapId);
  SelectAssetWidget<Gfx::Shader>(&skybox.mShaderId);
}

} // namespace Editor
