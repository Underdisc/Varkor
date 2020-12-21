#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <sstream>
#include <stb/stb_image.h>

#include "Error.h"
#include "Texture.h"

namespace Gfx {

Texture::Texture(const char* textureFile, TextureType type):
  mType(type), mFile(textureFile)
{
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data =
    stbi_load(textureFile, &mWidth, &mHeight, &mChannels, 0);
  if (!data)
  {
    std::stringstream reason;
    reason << textureFile << " loading failed: " << stbi_failure_reason();
    LogError(reason.str().c_str());
    mId = 0;
    return;
  }

  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_2D, mId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Use the number of channels to find the format of the image and upload the
  // image to the gpu.
  int format;
  if (mChannels == 4)
  {
    format = GL_RGBA;
  } else
  {
    format = GL_RGB;
  }
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    format,
    mWidth,
    mHeight,
    0,
    format,
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

TextureType Texture::Type() const
{
  return mType;
}

} // namespace Gfx
