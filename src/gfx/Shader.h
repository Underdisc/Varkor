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
  static const char* smTypeStrings[];
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
  Type mType;
  GLint mLocation;
};

struct Shader
{
public:
  struct InitInfo
  {
    Ds::Vector<std::string> mFiles;
    void Prep(const std::string& file);
    template<typename... Args>
    void Prep(const std::string& file, Args&&... args);
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

  Result Init(const std::string& file);
  Result Init(const std::string& vertexFile, const std::string& fragmentFile);
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
  GLuint mId;
  Ds::Vector<Uniform> mUniforms;

  static bool smLogMissingUniforms;
  static const char* smVersionHeader;
  static constexpr GLint smInvalidLocation = -1;
  static const char* smShaderTypeStrings[];
  static const GLenum smShaderTypes[];
  static const int smShaderTypeCount;

  // Tracks where a chunk of source code came from.
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

  // The information needed when compiling shader source code.
  struct CompileInfo
  {
    // The source code and the file it comes from.
    std::string mSource;
    std::string mFile;
    // The number of lines above the source code in the file.
    int mExcludedLines;
    GLenum mShaderType;
  };

  void InitializeUniforms();
  ValueResult<std::string> GetFileContent(const std::string& filename);
  int GetLineNumber(size_t until, const std::string& string);
  int GetChunkIndex(int lineNumber, const Ds::Vector<SourceChunk>& chunks);
  Result HandleIncludes(std::string* content, Ds::Vector<SourceChunk>* chunks);
  Result Compile(CompileInfo* compileInfo, GLuint shaderId);
  Result CreateProgram(Ds::Vector<CompileInfo>* allCompileInfo);
  ValueResult<Ds::Vector<CompileInfo>> CollectCompileInfo(
    const std::string& file);
  Result CreateProgram(const Ds::Vector<std::string>& files);
};

template<typename... Args>
void Shader::InitInfo::Prep(const std::string& file, Args&&... args)
{
  mFiles.Push(file);
  Prep(args...);
}

} // namespace Gfx

#endif
