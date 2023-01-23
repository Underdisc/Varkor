#include <glad/glad.h>

#include "Viewport.h"
#include "gfx/Framebuffer.h"

namespace Gfx {

Framebuffer::Framebuffer(): mFbo(0), mColorTbo(0), mDepthTbo(0) {}

Framebuffer::Framebuffer(const Options& options):
  mFbo(0), mColorTbo(0), mDepthTbo(0)
{
  Init(options);
}

Framebuffer::~Framebuffer()
{
  Purge();
}

void Framebuffer::Resize(int width, int height)
{
  mOptions.mWidth = width;
  mOptions.mHeight = height;
  Init(mOptions);
}

void Framebuffer::Init(const Options& options)
{
  Purge();

  mOptions = options;
  glGenFramebuffers(1, &mFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

  glGenTextures(1, &mColorTbo);
  glGenTextures(1, &mDepthTbo);
  if (mOptions.mMultisample) {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mColorTbo);
    glTexImage2DMultisample(
      GL_TEXTURE_2D_MULTISAMPLE,
      4,
      mOptions.mInternalFormat,
      mOptions.mWidth,
      mOptions.mHeight,
      GL_TRUE);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D_MULTISAMPLE,
      mColorTbo,
      0);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mDepthTbo);
    glTexImage2DMultisample(
      GL_TEXTURE_2D_MULTISAMPLE,
      4,
      GL_DEPTH_COMPONENT,
      mOptions.mWidth,
      mOptions.mHeight,
      GL_TRUE);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT,
      GL_TEXTURE_2D_MULTISAMPLE,
      mDepthTbo,
      0);
  }
  else {
    glBindTexture(GL_TEXTURE_2D, mColorTbo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      mOptions.mInternalFormat,
      mOptions.mWidth,
      mOptions.mHeight,
      0,
      mOptions.mFormat,
      mOptions.mPixelType,
      nullptr);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorTbo, 0);
    glBindTexture(GL_TEXTURE_2D, mDepthTbo);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_DEPTH_COMPONENT,
      mOptions.mWidth,
      mOptions.mHeight,
      0,
      GL_DEPTH_COMPONENT,
      GL_UNSIGNED_INT,
      nullptr);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTbo, 0);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::Purge()
{
  glDeleteFramebuffers(1, &mFbo);
  glDeleteTextures(1, &mColorTbo);
  glDeleteTextures(1, &mDepthTbo);

  mFbo = 0;
  mColorTbo = 0;
  mDepthTbo = 0;
}

GLuint Framebuffer::Fbo() const
{
  return mFbo;
}

GLuint Framebuffer::ColorTbo() const
{
  return mColorTbo;
}

GLenum Framebuffer::Format() const
{
  return mOptions.mFormat;
}

GLenum Framebuffer::PixelType() const
{
  return mOptions.mPixelType;
}

} // namespace Gfx
