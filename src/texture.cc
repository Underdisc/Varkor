#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb/stb_image.h>

#include "error.h"
#include "texture.h"

Texture::Texture(const char* textureFile)
{
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data =
    stbi_load(textureFile, &mWidth, &mHeight, &mChannels, 0);
  if (!data)
  {
    LogError(stbi_failure_reason());
    mId = 0;
    return;
  }

  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_2D, mId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    GL_RGBA,
    mWidth,
    mHeight,
    0,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

Texture::~Texture() {}

int Texture::Width() const
{
  return mWidth;
}

int Texture::Height() const
{
  return mHeight;
}

int Texture::Channels() const
{
  return mChannels;
}

unsigned int Texture::Id() const
{
  return mId;
}
