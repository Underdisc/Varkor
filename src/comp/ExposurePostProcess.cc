#include <glad/glad.h>
#include <imgui/imgui.h>

#include "comp/ExposurePostProcess.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"

AssetId ExposurePostProcess::smShaderId;

void ExposurePostProcess::VStaticInit()
{
  smShaderId = AssLib::Create<Gfx::Shader>(
    "ExposurePostProcess",
    "vres/shader/FullscreenTexture.glv",
    "vres/shader/ExposurePostProcess.glf");
}

void ExposurePostProcess::VInit(const World::Object& owner)
{
  mExposure = smDefaultExposure;
}

void ExposurePostProcess::VSerialize(Vlk::Value& val)
{
  val("Exposure") = mExposure;
}

void ExposurePostProcess::VDeserialize(const Vlk::Explorer& ex)
{
  mExposure = ex("Exposure").As<float>(smDefaultExposure);
}

void ExposurePostProcess::VRender(const World::Object& owner)
{
  const Gfx::Shader* shader = AssLib::TryGetLive<Gfx::Shader>(smShaderId);
  if (shader == nullptr) {
    return;
  }

  GLint samplerLoc = shader->UniformLocation(Gfx::Uniform::Type::Sampler);
  glUseProgram(shader->Id());
  glUniform1i(samplerLoc, 0);
  shader->SetUniform("uExposure", mExposure);
  Gfx::Renderer::RenderCurrentSpaceFramebuffer();
}

void ExposurePostProcess::VEdit(const World::Object& owner)
{
  ImGui::DragFloat("Exposure", &mExposure, 0.01f, 0.0f);
}