#include <glad/glad.h>
#include <sstream>

#include "gfx/Cubemap.h"
#include "gfx/Image.h"
#include "gfx/UniformVector.h"
#include "math/Vector.h"
#include "rsl/Library.h"

namespace Gfx {

UniformTypeId GetUniformTypeId(const std::string& name) {
  for (size_t i = 0; i < (size_t)UniformTypeId::Count; ++i) {
    const UniformTypeData& typeData = GetUniformTypeData((UniformTypeId)i);
    if (name == typeData.mName) {
      return (UniformTypeId)i;
    }
  }
  return UniformTypeId::Invalid;
}

UniformTypeData nUniformTypeData[(int)UniformTypeId::Count];

UniformTypeData::UniformTypeData(): mName(nullptr), mSize(0) {}

const UniformTypeData& GetUniformTypeData(UniformTypeId typeId) {
  if ((int)typeId < 0 || UniformTypeId::Count <= typeId) {
    std::stringstream error;
    error << "UniformTypeId \"" << (int)typeId << "\" invalid.";
    LogAbort(error.str().c_str());
  }
  return nUniformTypeData[(int)typeId];
}

void UniformVector::Init() {
  UniformType<int>::RegisterAttached(UniformTypeId::Int, "Int");
  UniformType<float>::RegisterAttached(UniformTypeId::Float, "Float");
  UniformType<Vec3>::RegisterAttached(UniformTypeId::Vec3, "Vec3");
  UniformType<Vec4>::RegisterAttached(UniformTypeId::Vec4, "Vec4");
  UniformType<ResId>::RegisterDetached(
    UniformTypeId::Texture2dRes, "Texture2dRes");
  UniformType<ResId>::RegisterDetached(
    UniformTypeId::TextureCubemapRes, "TextureCubemapRes");
  UniformType<GLuint>::RegisterDetached(UniformTypeId::Texture2d, "Texture2d");
  UniformType<GLuint>::RegisterDetached(
    UniformTypeId::TextureCubemap, "TextureCubemap");
}

UniformVector::UniformVector() {
  mData = nullptr;
  mSize = 0;
}

UniformVector::UniformVector(UniformVector&& other) {
  *this = std::move(other);
}

UniformVector& UniformVector::operator=(UniformVector&& other) {
  mData = other.mData;
  mSize = other.mSize;
  mUniformDescs = std::move(other.mUniformDescs);

  other.mData = nullptr;
  other.mSize = 0;

  return *this;
}

UniformVector::~UniformVector() {
  if (mData == nullptr) {
    return;
  }
  DestructUniforms();
  delete[] mData;
}

void* UniformVector::Add(UniformTypeId typeId, const std::string& name) {
  const UniformTypeData& typeData = GetUniformTypeData(typeId);
  size_t oldSize = mSize;
  size_t newSize = mSize + typeData.mSize;

  UniformDescriptor newUniformDesc;
  newUniformDesc.mName = name;
  newUniformDesc.mTypeId = typeId;
  if (mData == nullptr) {
    mData = alloc char[newSize];
    mSize = newSize;
    newUniformDesc.mByteIndex = 0;
  }
  else {
    char* newData = alloc char[newSize];
    for (const UniformDescriptor& uniformDesc: mUniformDescs) {
      void* oldUniformData = (void*)(mData + uniformDesc.mByteIndex);
      void* newUniformData = (void*)(newData + uniformDesc.mByteIndex);
      const UniformTypeData& currentTypeData =
        GetUniformTypeData(uniformDesc.mTypeId);
      currentTypeData.mMoveConstruct(oldUniformData, newUniformData);
    }
    DestructUniforms();
    delete[] mData;
    mData = newData;
    mSize = newSize;
    newUniformDesc.mByteIndex = oldSize;
  }
  void* uniformData = (void*)(mData + newUniformDesc.mByteIndex);
  typeData.mDefaultConstruct(uniformData);
  mUniformDescs.Push(std::move(newUniformDesc));
  return uniformData;
}

void* UniformVector::Get(UniformTypeId typeId, const std::string& name) {
  for (const UniformDescriptor& uniformDesc: mUniformDescs) {
    if (uniformDesc.mTypeId == typeId && uniformDesc.mName == name) {
      return (void*)(mData + uniformDesc.mByteIndex);
    }
  }
  const UniformTypeData& typeData = GetUniformTypeData(typeId);
  std::string error =
    "Uniform \"" + name + "\" of type \"" + typeData.mName + "\" not found.";
  LogAbort(error.c_str());
  return nullptr;
}

void UniformVector::Bind(const Shader& shader) const {
  int textureIndex = 0;
  Bind(shader, &textureIndex);
}

void UniformVector::Bind(const Shader& shader, int* textureIndex) const {
  for (const UniformDescriptor& uniformDesc: mUniformDescs) {
    switch (uniformDesc.mTypeId) {
    case UniformTypeId::Int: BindUniform<int>(shader, uniformDesc); break;
    case UniformTypeId::Float: BindUniform<float>(shader, uniformDesc); break;
    case UniformTypeId::Vec3: BindUniform<Vec3>(shader, uniformDesc); break;
    case UniformTypeId::Vec4: BindUniform<Vec4>(shader, uniformDesc); break;
    case UniformTypeId::Texture2dRes:
      BindTexture2DRes(shader, textureIndex, uniformDesc);
      break;
    case UniformTypeId::TextureCubemapRes:
      BindTextureCubemapRes(shader, textureIndex, uniformDesc);
      break;
    case UniformTypeId::Texture2d:
      BindTexture(shader, textureIndex, uniformDesc, GL_TEXTURE_2D);
      break;
    case UniformTypeId::TextureCubemap:
      BindTexture(shader, textureIndex, uniformDesc, GL_TEXTURE_CUBE_MAP);
      break;
    default: break;
    }
  }
}

void UniformVector::DestructUniforms() {
  for (const UniformDescriptor& uniformDesc: mUniformDescs) {
    void* uniformData = (void*)(mData + uniformDesc.mByteIndex);
    const UniformTypeData& uniformTypeData =
      GetUniformTypeData(uniformDesc.mTypeId);
    uniformTypeData.mDestruct(uniformData);
  }
}

void UniformVector::BindTexture(
  const Shader& shader,
  int* textureIndex,
  const UniformDescriptor& uniformDesc,
  GLenum target) const {
  char* uniformData = mData + uniformDesc.mByteIndex;
  BindTexture(shader, textureIndex, uniformDesc, target, *(GLuint*)uniformData);
}

void UniformVector::BindTexture(
  const Shader& shader,
  int* textureIndex,
  const UniformDescriptor& uniformDesc,
  GLenum target,
  GLuint id) const {
  glActiveTexture(GL_TEXTURE0 + *textureIndex);
  glBindTexture(target, id);
  shader.SetUniform(uniformDesc.mName.c_str(), *textureIndex);
  ++(*textureIndex);
}

void UniformVector::BindTexture2DRes(
  const Shader& shader,
  int* textureIndex,
  const UniformDescriptor& uniformDesc) const {
  const ResId& imageId = *(ResId*)(mData + uniformDesc.mByteIndex);
  Gfx::Image* image = Rsl::TryGetRes<Gfx::Image>(imageId);
  if (image == nullptr) {
    return;
  }
  BindTexture(shader, textureIndex, uniformDesc, GL_TEXTURE_2D, image->Id());
}

void UniformVector::BindTextureCubemapRes(
  const Shader& shader,
  int* textureIndex,
  const UniformDescriptor& uniformDesc) const {
  const ResId& cubemapId = *(ResId*)(mData + uniformDesc.mByteIndex);
  Gfx::Cubemap* cubemap = Rsl::TryGetRes<Gfx::Cubemap>(cubemapId);
  if (cubemap == nullptr) {
    return;
  }
  BindTexture(
    shader, textureIndex, uniformDesc, GL_TEXTURE_CUBE_MAP, cubemap->Id());
}

} // namespace Gfx
