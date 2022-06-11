#include <sstream>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

#include "AssetLibrary.h"
#include "gfx/Image.h"

namespace Gfx {

void Image::InitInfo::Serialize(Vlk::Value& val) const
{
  val("File") = mFile;
}

void Image::InitInfo::Deserialize(const Vlk::Explorer& ex)
{
  mFile = ex("File").As<std::string>("");
}

void Image::Purge()
{
  glDeleteTextures(1, &mId);
  mId = 0;
}

Image::Image(): mId(0) {}

Image::Image(Image&& other)
{
  *this = std::forward<Image>(other);
}

Image& Image::operator=(Image&& other)
{
  mWidth = other.mWidth;
  mHeight = other.mHeight;
  mId = other.mId;

  other.mId = 0;

  return *this;
}

Image::~Image()
{
  Purge();
}

Result Image::Init(const InitInfo& info)
{
  // Resolve the resource path.
  ValueResult<std::string> resolutionResult =
    AssLib::ResolveResourcePath(info.mFile);
  if (!resolutionResult.Success()) {
    return Result(resolutionResult.mError);
  }
  std::string path = resolutionResult.mValue;

  stbi_set_flip_vertically_on_load(true);
  int channels;
  unsigned char* data =
    stbi_load(path.c_str(), &mWidth, &mHeight, &channels, 0);
  if (!data) {
    std::stringstream error;
    error << "Failed to load \"" << path << "\": " << stbi_failure_reason();
    return Result(error.str());
  }

  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_2D, mId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum format;
  if (channels == 4) {
    format = GL_RGBA;
  }
  else {
    format = GL_RGB;
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
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
  glFinish();
  return Result();
}

Result Image::Init(const std::string& file)
{
  InitInfo info;
  info.mFile = file;
  return Init(info);
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
