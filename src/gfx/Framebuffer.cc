#include <glad/glad.h>

#include "Framebuffer.h"

namespace Gfx {

Framebuffer::Framebuffer(int width, int height):
  mFormat(GL_RED_INTEGER), mPixelType(GL_INT)
{
  glGenFramebuffers(1, &mFbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

  // Create the color attachment.
  glGenTextures(1, &mColorTbo);
  glBindTexture(GL_TEXTURE_2D, mColorTbo);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_R32I,
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

Framebuffer::~Framebuffer()
{
  glDeleteFramebuffers(1, &mFbo);
  glDeleteTextures(1, &mColorTbo);
  glDeleteTextures(1, &mDepthTbo);
}

unsigned int Framebuffer::Fbo()
{
  return mFbo;
}

GLenum Framebuffer::Format()
{
  return mFormat;
}

GLenum Framebuffer::PixelType()
{
  return mPixelType;
}

} // namespace Gfx
