#ifndef gfx_UniformArray_h
#define gfx_UniformArray_h

#include <string>

#include "ds/Vector.h"
#include "gfx/Shader.h"

namespace Gfx {

enum class UniformTypeId
{
  Int,
  Float,
  Vec3,
  Vec4,
  Texture2dRes,
  TextureCubemapRes,
  Texture2d,
  TextureCubemap,
  Count,
  Invalid,
};
template<typename T>
struct UniformType
{
  static UniformTypeId smTypeId;
  // Attached implies a one to one relationship between T and UniformTypeId.
  static void RegisterAttached(UniformTypeId typeId, const char* name);
  // Detached implies a one to many relationship between T and UniformTypeId.
  static void RegisterDetached(UniformTypeId typeId, const char* name);
};
template<typename T>
UniformTypeId GetUniformTypeId();
UniformTypeId GetUniformTypeId(const std::string& name);

struct UniformTypeData
{
  UniformTypeData();
  const char* mName;
  size_t mSize;
  void (*mDefaultConstruct)(void* at);
  void (*mMoveConstruct)(void* from, void* to);
  void (*mDestruct)(void* at);
};
template<typename T>
const UniformTypeData& GetUniformTypeData();
const UniformTypeData& GetUniformTypeData(UniformTypeId typeId);

struct UniformVector
{
  static void Init();

  struct UniformDescriptor
  {
    std::string mName;
    UniformTypeId mTypeId;
    size_t mByteIndex;
  };
  Ds::Vector<UniformDescriptor> mUniformDescs;
  char* mData;
  size_t mSize;

  UniformVector();
  UniformVector(UniformVector&& other);
  UniformVector& operator=(UniformVector&& other);
  ~UniformVector();

  template<typename T>
  T& Add(const std::string& name);
  template<typename T>
  void Add(const std::string& name, const T& value);
  template<typename T>
  T& Add(UniformTypeId typeId, const std::string& name);
  template<typename T>
  void Add(UniformTypeId typeId, const std::string& name, const T& value);
  void* Add(UniformTypeId typeId, const std::string& name);
  template<typename T>
  T& Get(const std::string& name);
  void* Get(UniformTypeId typeId, const std::string& name);
  void Bind(const Shader& shader) const;
  void Bind(const Shader& shader, int* textureIndex) const;

private:
  void DestructUniforms();
  template<typename T>
  void BindUniform(
    const Shader& shader, const UniformDescriptor& uniformDesc) const;
  void BindTexture(
    const Shader& shader,
    int* textureIndex,
    const UniformDescriptor& uniformDesc,
    GLenum target) const;
  void BindTexture(
    const Shader& shader,
    int* textureIndex,
    const UniformDescriptor& uniformDesc,
    GLenum target,
    GLuint id) const;
  void BindTexture2DRes(
    const Shader& shader,
    int* textureIndex,
    const UniformDescriptor& uniformDesc) const;
  void BindTextureCubemapRes(
    const Shader& shader,
    int* textureIndex,
    const UniformDescriptor& uniformDesc) const;
};

} // namespace Gfx

#include "gfx/UniformVector.hh"

#endif