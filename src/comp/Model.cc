#include <glad/glad.h>

#include "comp/AlphaColor.h"
#include "comp/Model.h"
#include "comp/Transform.h"
#include "editor/AssetInterfaces.h"
#include "gfx/Image.h"
#include "gfx/Material.h"
#include "gfx/Model.h"
#include "gfx/Shader.h"

namespace Comp {

void Model::VInit(const World::Object& owner)
{
  mModelId = AssLib::nDefaultAssetId;
  mShaderId = AssLib::nDefaultAssetId;
  mVisible = true;
}

void Model::VSerialize(Vlk::Value& modelVal)
{
  modelVal("ShaderId") = mShaderId;
  modelVal("ModelId") = mModelId;
  modelVal("Visible") = mVisible;
}

void Model::VDeserialize(const Vlk::Explorer& modelEx)
{
  mShaderId = modelEx("ShaderId").As<int>(AssLib::nDefaultAssetId);
  mModelId = modelEx("ModelId").As<int>(AssLib::nDefaultAssetId);
  mVisible = modelEx("Visible").As<bool>(true);
}

void Model::VRender(const World::Object& owner)
{
  const Gfx::Shader* shader = AssLib::TryGetLive<Gfx::Shader>(mShaderId);
  if (shader == nullptr) {
    return;
  }

  RenderOptions options;
  options.mShader = shader;
  Render(owner, options);
}

void Model::VEdit(const World::Object& owner)
{
  Editor::DropAssetWidget<Gfx::Model>(&mModelId);
  Editor::DropAssetWidget<Gfx::Shader>(&mShaderId);
  ImGui::Checkbox("Visible", &mVisible);
}

void Model::Render(
  const World::Object& owner, const RenderOptions& options) const
{
  if (!mVisible) {
    return;
  }

  const Gfx::Model* model = AssLib::TryGetLive<Gfx::Model>(mModelId);
  if (model == nullptr) {
    return;
  }

  const Gfx::Shader& shader = *options.mShader;
  GLint modelLoc = shader.UniformLocation(Gfx::Uniform::Type::Model);
  GLint diffuseLoc = shader.UniformLocation(Gfx::Uniform::Type::ADiffuse);
  GLint specLoc = shader.UniformLocation(Gfx::Uniform::Type::ASpecular);
  GLint skyboxSamplerLoc =
    shader.UniformLocation(Gfx::Uniform::Type::SkyboxSampler);

  glUseProgram(shader.Id());
  Comp::AlphaColor* alphaColorComp = owner.TryGet<Comp::AlphaColor>();
  if (alphaColorComp != nullptr) {
    GLint alphaColorLoc =
      shader.UniformLocation(Gfx::Uniform::Type::AlphaColor);
    glUniform4fv(alphaColorLoc, 1, alphaColorComp->mColor.CData());
  }
  glUniform1i(skyboxSamplerLoc, 0);

  auto& transform = owner.Get<Comp::Transform>();
  Mat4 memberTransformation = transform.GetWorldMatrix(owner);

  for (const Gfx::Model::DrawInfo& drawInfo : model->GetAllDrawInfo()) {
    // Prepare all of the textures.
    const Gfx::Material& material = model->GetMaterial(drawInfo.mMaterialIndex);
    int textureCounter = 0;
    Ds::Vector<GLint> indices;
    for (const Gfx::Material::TextureGroup& group : material.mGroups) {
      indices.Reserve(group.mImages.Size());
      for (const Gfx::Image& image : group.mImages) {
        glActiveTexture(GL_TEXTURE0 + textureCounter);
        glBindTexture(GL_TEXTURE_2D, image.Id());
        indices.Push(textureCounter);
        ++textureCounter;
      }
      switch (group.mType) {
      case Gfx::Material::TextureType::Diffuse:
        glUniform1iv(diffuseLoc, (GLsizei)indices.Size(), indices.CData());
        break;
      case Gfx::Material::TextureType::Specular:
        glUniform1iv(specLoc, (GLsizei)indices.Size(), indices.CData());
        break;
      }
      indices.Clear();
    }

    // Render the mesh.
    Mat4 transformation = memberTransformation * drawInfo.mTransformation;
    glUniformMatrix4fv(modelLoc, 1, true, transformation.CData());
    const Gfx::Mesh& mesh = model->GetMesh(drawInfo.mMeshIndex);
    glBindVertexArray(mesh.Vao());
    glDrawElements(
      GL_TRIANGLES, (GLsizei)mesh.IndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
}

} // namespace Comp
