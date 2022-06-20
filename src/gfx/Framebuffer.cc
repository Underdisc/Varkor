#include <glad/glad.h>

#include "Viewport.h"
#include "gfx/Framebuffer.h"

namespace Gfx {

Framebuffer::Framebuffer(
  int width, int height, unsigned int format, unsigned int pixelType):
  mFormat(format), mPixelType(pixelType)
{
  Init(width, height);
}

Framebuffer::~Framebuffer()
{
  Purge();
}

void Framebuffer::Resize(int width, int height)
{
  Purge();
  Init(width, height);
}

void Framebuffer::Init(int width, int height)
{
  glGenFramebuffers(1, &mFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

  // Create the color attachment.
  GLint internalFormat;
  switch (mFormat) {
  case GL_RED_INTEGER: internalFormat = GL_R32I; break;
  case GL_RGBA: internalFormat = GL_RGBA; break;
  }
  glGenTextures(1, &mColorTbo);
  glBindTexture(GL_TEXTURE_2D, mColorTbo);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    internalFormat,
    width,
    height,
    0,
    mFormat,
    mPixelType,
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
    width,
    height,
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

unsigned int Framebuffer::Fbo() const
{
  return mFbo;
}

unsigned int Framebuffer::ColorTbo() const
{
  return mColorTbo;
}

unsigned int Framebuffer::Format() const
{
  return mFormat;
}

unsigned int Framebuffer::PixelType() const
{
  return mPixelType;
}

} // namespace Gfx
