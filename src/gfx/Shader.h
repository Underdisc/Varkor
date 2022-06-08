#ifndef gfx_Shader_h
#define gfx_Shader_h

#include <glad/glad.h>
#include <string>

#include "Result.h"
#include "ds/Vector.h"
#include "math/Matrix4.h"
#include "math/Vector.h"
#include "vlk/Valkor.h"

namespace Gfx {

struct Uniform
{
  enum class Type
  {
    Model = 0,
    Sampler,
    Color,
    AlphaColor,
    MemberId,
    FillAmount,
    SkyboxSampler,
    ADiffuse,
    ASpecular,
    Count
  };
  constexpr static char* smTypeStrings[] {
    "uModel",
    "uTexture",
    "uColor",
    "uAlphaColor",
    "uMemberId",
    "uFillAmount",
    "uSkyboxSampler",
    "uMateial.mDiffuse",
    "uMaterial.mSpecular"};
  Type mType;
  GLint mLocation;
};

struct Shader
{
public:
  struct InitInfo
  {
    std::string mVertexFile;
    std::string mFragmentFile;
    void Prep(const char* vertexFile, const char* fragmentFile);
    void Serialize(Vlk::Value& val) const;
    void Deserialize(const Vlk::Explorer& ex);
  };
  Result Init(const InitInfo& info);
  void Finalize() {};
  void Purge();

  Shader();
  Shader(Shader&& other);
  Shader& operator=(Shader&& other);
  ~Shader();

  Result Init(const char* vertexFile, const char* fragmentFile);
  GLuint Id() const;
  GLint UniformLocation(Uniform::Type type) const;
  GLint UniformLocation(const char* name) const;
  void Use() const;
  void SetUniform(const char* name, float value) const;
  void SetUniform(const char* name, int value) const;
  void SetUniform(const char* name, const Vec3& value) const;
  void SetUniform(const char* name, const Vec4& value) const;
  void SetUniform(const char* name, const Mat4& value) const;

private:
  GLuint mProgram;
  Ds::Vector<Uniform> mUniforms;

  static bool smLogMissingUniforms;
  static constexpr GLint smInvalidLocation = -1;
  static constexpr char* smVersionHeader = "#version 330 core\n";

  struct SourceChunk
  {
    // The file that the chunk comes from.
    std::string mFile;
    // The line numbers that the chunk of source code starts and ends on.
    // [start, end). The start is inclusive and the end is not.
    int mStartLine;
    int mEndLine;
    // The number of lines above this chunk in the original source file that it
    // comes from.
    int mExcludedLines;
  };
  struct IncludeResult
  {
    bool mSuccess;
    std::string mError;
    Ds::Vector<SourceChunk> mChunks;
  };
  int GetChunkIndex(int lineNumber, const Ds::Vector<SourceChunk>& chunks);
  Shader::IncludeResult HandleIncludes(
    const std::string& file, std::string* content);
  Result Compile(
    const std::string& filename, int shaderType, unsigned int* shaderId);
  void InitializeUniforms();
};

} // namespace Gfx

#endif
