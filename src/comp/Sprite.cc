#include "comp/Sprite.h"
#include "comp/AlphaColor.h"
#include "comp/Transform.h"
#include "gfx/Image.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"

namespace Comp {

void Sprite::VInit(const World::Object& owner)
{
  mImageId = AssLib::nDefaultAssetId;
  mShaderId = AssLib::nDefaultAssetId;
}

void Sprite::VSerialize(Vlk::Value& spriteVal)
{
  spriteVal("ImageId") = mImageId;
  spriteVal("ShaderId") = mShaderId;
}

void Sprite::VDeserialize(const Vlk::Explorer& spriteEx)
{
  mImageId = spriteEx("ImageId").As<AssetId>(AssLib::nDefaultAssetId);
  mShaderId = spriteEx("ShaderId").As<AssetId>(AssLib::nDefaultAssetId);
}

void Sprite::VRender(const World::Object& owner)
{
  const Gfx::Shader* shader =
    AssLib::TryGetLive<Gfx::Shader>(mShaderId, AssLib::nDefaultSpriteShaderId);
  if (shader == nullptr) {
    return;
  }

  RenderOptions options;
  options.mShader = shader;
  Render(owner, options);
}
void Sprite::Render(
  const World::Object& owner, const RenderOptions& options) const
{
  const Gfx::Image* image = AssLib::TryGetLive<Gfx::Image>(mImageId);
  if (image == nullptr) {
    return;
  }

  const Gfx::Shader& shader = *options.mShader;
  GLint modelLoc = shader.UniformLocation(Gfx::Uniform::Type::Model);
  GLint samplerLoc = shader.UniformLocation(Gfx::Uniform::Type::Sampler);

  glUseProgram(shader.Id());
  glUniform1i(samplerLoc, 0);

  auto& transform = owner.Get<Comp::Transform>();
  Mat4 aspectScale;
  Math::Scale(&aspectScale, {image->Aspect(), 1.0f, 1.0f});
  Mat4 transformation = transform.GetWorldMatrix(owner) * aspectScale;
  glUniformMatrix4fv(modelLoc, 1, true, transformation.CData());

  auto* alphaColorComp = owner.TryGet<Comp::AlphaColor>();
  if (alphaColorComp != nullptr) {
    GLint alphaColorLoc =
      shader.UniformLocation(Gfx::Uniform::Type::AlphaColor);
    glUniform4fv(alphaColorLoc, 1, alphaColorComp->mColor.CData());
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, image->Id());
  Gfx::Renderer::PrepShadowState(*owner.mSpace, shader, 1, 1);
  Gfx::Renderer::RenderQuad(Gfx::Renderer::nSpriteVao);
  Gfx::Renderer::UnbindTextures(0, 2);
}

} // namespace Comp
