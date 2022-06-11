#include <stb_image.h>
#include <utility>

#include "AssetLibrary.h"
#include "gfx/Cubemap.h"

namespace Gfx {

Cubemap::InitInfo::InitInfo(): mFilter(GL_LINEAR) {}

void Cubemap::InitInfo::Serialize(Vlk::Value& val) const
{
  for (int i = 0; i < 6; ++i) {
    val(smFileDescriptors[i]) = mFiles[i];
  }
  Vlk::Value& filterVal = val("Filter");
  switch (mFilter) {
  case GL_LINEAR: filterVal = smFilterTypes[0];
  case GL_NEAREST: filterVal = smFilterTypes[1];
  }
}

void Cubemap::InitInfo::Deserialize(const Vlk::Explorer& ex)
{
  for (int i = 0; i < 6; ++i) {
    mFiles[i] = ex(smFileDescriptors[i]).As<std::string>("");
  }
  std::string filterType = ex("Filter").As<std::string>(smFilterTypes[0]);
  if (filterType == smFilterTypes[1]) {
    mFilter = GL_LINEAR;
  }
  else {
    mFilter = GL_NEAREST;
  }
}

Result Cubemap::Init(const std::string& file)
{
  // This Init function was made specifically for the default cubemap. It uses
  // the same 9 pixel image for each side.
  InitInfo info;
  for (int i = 0; i < 6; ++i) {
    info.mFiles[i] = file;
  }
  info.mFilter = GL_NEAREST;
  return Init(info);
}

Result Cubemap::Init(const InitInfo& info)
{
  Purge();

  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mId);
  unsigned char* data;
  for (int i = 0; i < 6; ++i) {
    // Resolve the path for the current face.
    ValueResult<std::string> resolutionResult =
      AssLib::ResolveResourcePath(info.mFiles[i]);
    if (!resolutionResult.Success()) {
      return Result(resolutionResult.mError);
    }
    std::string path = resolutionResult.mValue;

    // Upload the texture for the current face.
    int width, height, channels;
    stbi_set_flip_vertically_on_load(false);
    data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
      std::stringstream error;
      error << "Failed to load \"" << path << "\": " << stbi_failure_reason();
      return Result(error.str());
    }
    GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
    GLenum format;
    if (channels == 4) {
      format = GL_RGBA;
    }
    else {
      format = GL_RGB;
    }
    glTexImage2D(
      face, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, info.mFilter);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, info.mFilter);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glFinish();
  return Result();
}

void Cubemap::Purge()
{
  glDeleteTextures(1, &mId);
  mId = 0;
}

Cubemap::Cubemap(): mId(0) {}

Cubemap::Cubemap(Cubemap&& other)
{
  *this = std::forward<Cubemap>(other);
}

Cubemap& Cubemap::operator=(Cubemap&& other)
{
  mId = other.mId;
  other.mId = 0;
  return *this;
}

Cubemap::~Cubemap()
{
  Purge();
}

GLuint Cubemap::Id() const
{
  return mId;
}

} // namespace Gfx