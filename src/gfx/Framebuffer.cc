#include <glad/glad.h>

#include "Viewport.h"
#include "gfx/Framebuffer.h"

namespace Gfx {

Framebuffer::Framebuffer(const Options& options): mOptions(options)
{
  Init();
}

Framebuffer::~Framebuffer()
{
  Purge();
}

void Framebuffer::Resize(int width, int height)
{
  Purge();
  mOptions.mWidth = width;
  mOptions.mHeight = height;
  Init();
}

void Framebuffer::Init()
{
  glGenFramebuffers(1, &mFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

  // Create the color attachment.
  glGenTextures(1, &mColorTbo);
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

  // Create the depth attachment.
  glGenTextures(1, &mDepthTbo);
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

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::Purge()
{
  glDeleteFramebuffers(1, &mFbo);
  glDeleteTextures(1, &mColorTbo);
  glDeleteTextures(1, &mDepthTbo);
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
