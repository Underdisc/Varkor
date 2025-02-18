#include <imgui/imgui.h>

#include "Viewport.h"
#include "comp/Camera.h"
#include "comp/ShadowMap.h"
#include "comp/Sprite.h"
#include "comp/Transform.h"
#include "editor/Utility.h"
#include "gfx/Renderer.h"
#include "gfx/Shader.h"

namespace Comp {

ShadowMap::~ShadowMap() {
  DeleteFramebuffer();
}

void ShadowMap::VInit(const World::Object& owner) {
  mWidth = smDefaultWidth;
  mHeight = smDefaultHeight;
  mBias = smDefaultBias;
  CreateFramebuffer();
}

void ShadowMap::VSerialize(Vlk::Value& val) {
  val("Width") = mWidth;
  val("Height") = mHeight;
  val("Bias") = mBias;
}

void ShadowMap::VDeserialize(const Vlk::Explorer& ex) {
  mWidth = ex("Width").As<unsigned int>(smDefaultWidth);
  mHeight = ex("Height").As<unsigned int>(smDefaultHeight);
  mBias = ex("Bias").As<float>(smDefaultBias);
  CreateFramebuffer();
}

void ShadowMap::VEdit(const World::Object& owner) {
  ImGui::PushItemWidth(-Editor::CalcBufferWidth("Height"));
  ImGui::SliderFloat("Bias", &mBias, -0.1f, 0.1f);
  int newWidth = (int)mWidth;
  ImGui::DragInt(
    "Width", &newWidth, 1, 1, 4'096, "%d", ImGuiSliderFlags_Logarithmic);
  int newHeight = (int)mHeight;
  ImGui::DragInt(
    "Height", &newHeight, 1, 1, 4'096, "%d", ImGuiSliderFlags_Logarithmic);
  if (newWidth != (int)mWidth || newHeight != (int)mHeight) {
    mWidth = (unsigned int)newWidth;
    mHeight = (unsigned int)newHeight;
    DeleteFramebuffer();
    CreateFramebuffer();
  }
  ImGui::PopItemWidth();
}

Mat4 ShadowMap::ProjView(const World::Object& owner) const {
  Comp::Transform& transform = owner.Get<Comp::Transform>();
  Comp::Camera& camera = owner.Get<Comp::Camera>();
  Mat4 view = transform.GetInverseWorldMatrix(owner);
  Mat4 proj = camera.Proj();
  return proj * view;
}

void ShadowMap::CreateFramebuffer() {
  glGenTextures(1, &mTbo);
  glBindTexture(GL_TEXTURE_2D, mTbo);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_DEPTH_COMPONENT,
    mWidth,
    mHeight,
    0,
    GL_DEPTH_COMPONENT,
    GL_FLOAT,
    NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Give all texels outside of the main texture the maximum depth value.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glGenFramebuffers(1, &mFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mTbo, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void ShadowMap::DeleteFramebuffer() {
  glDeleteTextures(1, &mTbo);
  glDeleteFramebuffers(1, &mFbo);
}

} // namespace Comp
