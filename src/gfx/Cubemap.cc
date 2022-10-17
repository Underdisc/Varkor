#include <imgui/imgui.h>
#include <stb_image.h>
#include <utility>

#include "editor/Utility.h"
#include "gfx/Cubemap.h"
#include "rsl/Library.h"

namespace Gfx {

Cubemap::FilterType Cubemap::GetFilterType(const std::string& filterTypeString)
{
  for (int i = 0; i < (int)FilterType::Count; ++i) {
    if (filterTypeString == smFilterTypeStrings[i]) {
      return (FilterType)i;
    }
  }
  return FilterType::Invalid;
}

Cubemap::Specification Cubemap::GetSpecification(
  const std::string& specificationString)
{
  for (int i = 0; i < (int)Specification::Count; ++i) {
    if (specificationString == smSpecificationStrings[i]) {
      return (Specification)i;
    }
  }
  return Specification::Invalid;
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
  glDeleteTextures(1, &mId);
  mId = 0;
}

Result Cubemap::Init(const Vlk::Explorer& configEx)
{
  Config config;

  // Get the filter type.
  Vlk::Explorer filterEx = configEx("Filter");
  if (!filterEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result("Missing :Filter: TrueValue");
  }
  std::string filterTypeString = filterEx.As<std::string>();
  FilterType filterType = GetFilterType(filterTypeString);
  if (filterType == FilterType::Invalid) {
    std::string error = "Filter \"" + filterTypeString + "\" is invalid.";
    return Result(error);
  }
  config.mGlFilter = smFilterTypeGlValues[(int)filterType];

  // Get the specification type.
  Vlk::Explorer specificationEx = configEx("Specification");
  if (!specificationEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result("Missing :Specification: TrueValue");
  }
  std::string specificationString = specificationEx.As<std::string>();
  if (config.mSpecification == Specification::Invalid) {
    std::string error =
      "Specification \"" + specificationString + "\" is invalid.";
    return Result(error);
  }
  config.mSpecification = GetSpecification(specificationString);

  // Get the face files depending on specification type.
  if (config.mSpecification == Specification::Split) {
    for (int i = 0; i < smFileDescriptorCount; ++i) {
      Vlk::Explorer fileEx = configEx(smFileDescriptorStrings[i]);
      if (!fileEx.Valid(Vlk::Value::Type::TrueValue)) {
        std::string error = "Missing :";
        error += smFileDescriptorStrings[i];
        error += ": TrueValue.";
        return Result(error);
      }
      config.mFaceFiles[i] = fileEx.As<std::string>();
    }
  }
  else if (config.mSpecification == Specification::Single) {
    Vlk::Explorer fileEx = configEx("File");
    if (!fileEx.Valid(Vlk::Value::Type::TrueValue)) {
      return Result("Missing :File: TrueValue");
    }
    config.mFaceFiles[0] = fileEx.As<std::string>();
  }

  // We have a valid configuration if we reach this point.
  Init(config);
  return Result();
}

Result Cubemap::Init(const Config& config)
{
  auto uploadFace = [](GLenum target, GLenum format, const Face& face)
  {
    glTexImage2D(
      target,
      0,
      format,
      face.mWidth,
      face.mHeight,
      0,
      format,
      GL_UNSIGNED_BYTE,
      face.mData);
  };

  glGenTextures(1, &mId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, mId);
  if (config.mSpecification == Specification::Split) {
    for (int i = 0; i < smFileDescriptorCount; ++i) {
      ValueResult<Face> result = Face::Init(config.mFaceFiles[i]);
      if (!result.Success()) {
        return result;
      }
      const Face& face = result.mValue;
      GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
      GLenum format = face.mChannels == 4 ? GL_RGBA : GL_RGB;
      uploadFace(target, format, face);
    }
  }
  else if (config.mSpecification == Specification::Single) {
    ValueResult<Face> result = Face::Init(config.mFaceFiles[0]);
    if (!result.Success()) {
      return result;
    }
    const Face& face = result.mValue;
    GLenum format = face.mChannels == 4 ? GL_RGBA : GL_RGB;
    for (int i = 0; i < smFileDescriptorCount; ++i) {
      GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
      uploadFace(target, format, face);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, config.mGlFilter);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, config.mGlFilter);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  return Result();
}

void Cubemap::EditConfig(Vlk::Value* configValP)
{
  // Edit the filter type.
  ImGui::PushItemWidth(-Editor::CalcBufferWidth("Specification"));
  Vlk::Value& configVal = *configValP;
  Vlk::Value& filterVal = configVal("Filter");
  std::string filterString = filterVal.As<std::string>(smFilterTypeStrings[0]);
  FilterType filterType = GetFilterType(filterString);
  int newFilterType = (int)filterType;
  ImGui::Combo(
    "Filter", &newFilterType, smFilterTypeStrings, (int)FilterType::Count);
  if (newFilterType != (int)filterType) {
    filterVal = smFilterTypeStrings[newFilterType];
  }

  // Edit the specification.
  Vlk::Value& specificationVal = configVal("Specification");
  std::string specificationString =
    specificationVal.As<std::string>(smSpecificationStrings[0]);
  Specification currentSpecification = GetSpecification(specificationString);
  int newSpecificationIndex = (int)currentSpecification;
  ImGui::Combo(
    "Specification",
    &newSpecificationIndex,
    smSpecificationStrings,
    (int)Specification::Count);
  if (newSpecificationIndex != (int)currentSpecification) {
    specificationVal = smSpecificationStrings[newSpecificationIndex];
    currentSpecification = (Specification)newSpecificationIndex;
  }
  ImGui::PopItemWidth();

  // Edit the file paths.
  if (currentSpecification == Specification::Split) {
    configVal.TryRemovePair("File");
    float bufferWidth = Editor::CalcBufferWidth(smFileDescriptorStrings[3]);
    for (int i = 0; i < smFileDescriptorCount; ++i) {
      Vlk::Value& fileVal = configVal(smFileDescriptorStrings[i]);
      std::string file = fileVal.As<std::string>("");
      // todo: This should accept drag and drop input from the file browser.
      Editor::InputText(smFileDescriptorStrings[i], &file, -bufferWidth);
      fileVal = file;
    }
  }
  else if (currentSpecification == Specification::Single) {
    configVal.TryRemovePair("Files");
    ImGui::Text("File");
    ImGui::SameLine();
    Vlk::Value& fileVal = configVal("File");
    std::string file = fileVal.As<std::string>("");
    // todo: drag and drop.
    Editor::InputText("File", &file, -Editor::CalcBufferWidth("File"));
    fileVal = file;
  }
}

GLuint Cubemap::Id() const
{
  return mId;
}

ValueResult<Cubemap::Face> Cubemap::Face::Init(const std::string& file)
{
  // Resolve the file path.
  ValueResult<std::string> resolutionResult = Rsl::ResolveResPath(file);
  if (!resolutionResult.Success()) {
    return resolutionResult;
  }
  const std::string& resolvedFile = resolutionResult.mValue;

  // Get the face data.
  Face face;
  stbi_set_flip_vertically_on_load(false);
  face.mData = stbi_load(
    resolvedFile.c_str(), &face.mWidth, &face.mHeight, &face.mChannels, 0);
  if (face.mData == nullptr) {
    std::string error =
      "Loading \"" + file + "\" failed.\n" + stbi_failure_reason();
    return ValueResult<Face>(Result(error));
  }
  return ValueResult<Face>(std::move(face));
}

Cubemap::Face::Face(): mData(nullptr) {}

Cubemap::Face::Face(Face&& other):
  mData(other.mData),
  mWidth(other.mWidth),
  mHeight(other.mHeight),
  mChannels(other.mChannels)
{
  other.mData = nullptr;
}

Cubemap::Face::~Face()
{
  if (mData != nullptr) {
    stbi_image_free(mData);
  }
}

} // namespace Gfx