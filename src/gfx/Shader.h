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

struct Shader
{
  Shader();
  Shader(Shader&& other);
  Shader& operator=(Shader&& other);
  ~Shader();

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

  enum class SubType
  {
    Vertex,
    Fragment,
    Count,
    Invalid,
  };
  constexpr static const char* smSubTypeStrings[] = {"vertex", "fragment"};
  constexpr static GLenum smGlSubTypes[] = {
    GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
  static SubType GetSubType(const std::string& subTypeString);

  // The information needed to compile subshader source code.
  struct CompileInfo
  {
    std::string mSource;
    SubType mSubType;
    // Chunks describing where each part of the source came from.
    Ds::Vector<SourceChunk> mChunks;
    int GetChunkIndex(int lineNumber) const;
  };

  static void EditConfig(Vlk::Value* configValP);
  Result Init(const Vlk::Explorer& configEx);
  Result Init(const Ds::Vector<std::string>& files);
  Result Init(const Ds::Vector<CompileInfo>& allCompileInfo);

  GLuint Id() const;
  GLint UniformLocation(const char* name) const;
  void Use() const;
  void SetUniform(const char* name, float value) const;
  void SetUniform(const char* name, int value) const;
  void SetUniform(const char* name, const Vec3& value) const;
  void SetUniform(const char* name, const Vec4& value) const;
  void SetUniform(const char* name, const Mat4& value) const;

  constexpr static bool smLogMissingUniforms = false;
  constexpr static GLint smInvalidLocation = -1;
  constexpr static const char* smVersionHeader = "#version 330 core\n";

private:
  GLuint mId;

  void InitializeUniforms();
  Result CompileSubShader(const CompileInfo& compileInfo, GLuint subShaderId);
  Result CreateProgram(const Ds::Vector<CompileInfo>& allCompileInfo);
  int GetLineNumber(size_t until, const std::string& string);
  VResult<std::string> GetFileContent(const std::string& filename);
  Result HandleIncludes(Gfx::Shader::CompileInfo* compileInfo);
  VResult<Ds::Vector<Gfx::Shader::CompileInfo>> CollectCompileInfo(
    const std::string& file);
};

} // namespace Gfx

#endif
