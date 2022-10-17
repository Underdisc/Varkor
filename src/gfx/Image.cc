#include <imgui/imgui.h>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include "editor/Utility.h"
#include "gfx/Image.h"
#include "rsl/Library.h"

namespace Gfx {

Image::Image(): mId(0) {}

Image::Image(Image&& other)
{
  *this = std::move(other);
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
  if (mId != 0) {
    glDeleteTextures(1, &mId);
    mId = 0;
  }
}

void Image::EditConfig(Vlk::Value* configValP)
{
  Vlk::Value& configVal = *configValP;
  Vlk::Value& fileVal = configVal("File");
  std::string file = fileVal.As<std::string>("");
  // todo: drag and drop
  Editor::InputText("File", &file, -Editor::CalcBufferWidth("File"));
  fileVal = file;
}

Result Image::Init(const Vlk::Explorer& configEx)
{
  Vlk::Explorer fileEx = configEx("File");
  if (!fileEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result("Missing :File: TrueValue");
  }
  std::string file = fileEx.As<std::string>();
  return Init(file);
}

Result Image::Init(const std::string& file)
{
  // Resolve the file path.
  ValueResult<std::string> resolutionResult = Rsl::ResolveResPath(file);
  if (!resolutionResult.Success()) {
    return Result(resolutionResult.mError);
  }
  std::string absoluteFile = resolutionResult.mValue;

  stbi_set_flip_vertically_on_load(true);
  Config config;
  int channels;
  config.mData = stbi_load(
    absoluteFile.c_str(), &config.mWidth, &config.mHeight, &channels, 0);
  if (config.mData == nullptr) {
    return Result(
      "Failed to load \"" + absoluteFile + "\"\n " + stbi_failure_reason());
  }

  channels == 4 ? config.mInternalFormat = GL_RGBA :
                  config.mInternalFormat = GL_RGB;
  config.mFormat = config.mInternalFormat;
  config.mPixelAlignment = 4;
  Init(config);
  stbi_image_free(config.mData);
  return Result();
}

void Image::Init(const Config& config)
{
  mHeight = config.mHeight;
  mWidth = config.mWidth;

  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_2D, mId);
  glPixelStorei(GL_UNPACK_ALIGNMENT, config.mPixelAlignment);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    config.mInternalFormat,
    mWidth,
    mHeight,
    0,
    config.mFormat,
    GL_UNSIGNED_BYTE,
    config.mData);
  glGenerateMipmap(GL_TEXTURE_2D);
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
