#include <sstream>
#include <stb_image.h>

#include "gfx/Image.h"

namespace Gfx {

Util::Result Image::Init(std::string paths[smInitPathCount])
{
  return Image::Init(paths[0]);
}

void Image::Purge()
{
  glDeleteTextures(1, &mId);
  mId = 0;
}

bool Image::Live() const
{
  return mId != 0;
}

Image::Image(): mId(0) {}

Image::~Image()
{
  Purge();
}

Util::Result Image::Init(const std::string& file)
{
  stbi_set_flip_vertically_on_load(true);
  int channels;
  unsigned char* data =
    stbi_load(file.c_str(), &mWidth, &mHeight, &channels, 0);
  if (!data)
  {
    std::stringstream error;
    error << file << " loading failed: " << stbi_failure_reason();
    return Util::Result(error.str());
  }

  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_2D, mId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum format;
  if (channels == 4)
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
  return Util::Result(true);
}

GLuint Image::Id() const
{
  return mId;
}

float Image::Aspect() const
{
  return (float)mWidth / (float)mHeight;
}

} // namespace Gfx
