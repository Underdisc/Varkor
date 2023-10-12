#include <imgui/imgui.h>
#include <utility>

#define DDSKTX_IMPLEMENT
#include <dds-ktx/dds-ktx.h>
#undef DDSKTX_IMPLEMENT
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
  Editor::DropResourceFileWidget("File", &file);
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
  VResult<std::string> resolutionResult = Rsl::ResolveResPath(file);
  if (!resolutionResult.Success()) {
    return Result(resolutionResult.mError);
  }
  std::string absoluteFile = resolutionResult.mValue;

  // Open the file and discover its type.
  FILE* fileStream = stbi__fopen(absoluteFile.c_str(), "rb");
  if (fileStream == nullptr) {
    return Result("File \"" + absoluteFile + "\" could not be opened.");
  }
  char typeIdentifier[5];
  fgets(typeIdentifier, 5, fileStream);
  fseek(fileStream, 0, SEEK_SET);

  // Handle DDS loading.
  if (strcmp(typeIdentifier, "DDS ") == 0) {
    Result result = InitDDS(fileStream);
    fclose(fileStream);
    return result;
  }

  // Handle all other image formats.
  int width, height, channels;
  void* imageData =
    stbi_load_from_file(fileStream, &width, &height, &channels, 0);
  fclose(fileStream);
  if (imageData == nullptr) {
    return Result(
      "File \"" + absoluteFile + "\" failed load.\n" + stbi_failure_reason());
  }
  Init(imageData, width, height, channels);
  stbi_image_free(imageData);
  return Result();
}

Result Image::InitDDS(FILE* stream)
{
  // Read the entire dds file stream.
  fseek(stream, 0, SEEK_END);
  long byteCount = ftell(stream);
  fseek(stream, 0, SEEK_SET);
  Ds::Vector<char> fileData;
  fileData.Reserve(byteCount);
  fread((void*)fileData.Data(), 1, byteCount, stream);

  // Parse the file data.
  ddsktx_texture_info textureInfo = {0};
  bool success =
    ddsktx_parse(&textureInfo, (void*)fileData.Data(), byteCount, nullptr);
  if (!success) {
    return Result("Parsing DDS data failed.");
  }

  // Find the file's pixel format.
  GLenum internalFormat;
  switch (textureInfo.format) {
  case DDSKTX_FORMAT_BC1:
    internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    break;
  case DDSKTX_FORMAT_BC3:
    internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    break;
  case DDSKTX_FORMAT_BC5:
    internalFormat = GL_COMPRESSED_RED_GREEN_RGTC2_EXT;
    break;
  default: return Result("DDS texture format not implemented.");
  }

  // Upload the texture and mipmaps.
  CreateTexutre();
  for (int i = 0; i < textureInfo.num_mips; ++i) {
    ddsktx_sub_data subInfo;
    ddsktx_get_sub(&textureInfo, &subInfo, fileData.Data(), byteCount, 0, 0, i);
    glCompressedTexImage2D(
      GL_TEXTURE_2D,
      i,
      internalFormat,
      subInfo.width,
      subInfo.height,
      0,
      subInfo.size_bytes,
      subInfo.buff);
  }
  return Result();
}

Result Image::Init(const void* fileData, int size)
{
  int width, height, channels;
  void* imageData = stbi_load_from_memory(
    (stbi_uc*)fileData, size, &width, &height, &channels, 0);
  if (imageData == nullptr) {
    return Result(stbi_failure_reason());
  }
  Init(imageData, width, height, channels);
  stbi_image_free(imageData);
  return Result();
}

Result Image::Init(const void* imageData, int width, int height, int channels)
{
  GLenum internalFormat;
  channels == 4 ? internalFormat = GL_RGBA : internalFormat = GL_RGB;
  GLenum format = internalFormat;
  GLint pixelAlignment = 4;
  return Init(imageData, width, height, internalFormat, format, pixelAlignment);
}

Result Image::Init(
  const void* imageData,
  int width,
  int height,
  GLenum internalFormat,
  GLenum format,
  GLint pixelAlignmet)
{
  mWidth = width;
  mHeight = height;
  CreateTexutre();
  glPixelStorei(GL_UNPACK_ALIGNMENT, pixelAlignmet);
  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    internalFormat,
    mWidth,
    mHeight,
    0,
    format,
    GL_UNSIGNED_BYTE,
    imageData);
  glGenerateMipmap(GL_TEXTURE_2D);
  return Result();
}

void Image::CreateTexutre()
{
  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_2D, mId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(
    GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
