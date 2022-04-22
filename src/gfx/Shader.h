#ifndef gfx_Shader_h
#define gfx_Shader_h

#include <glad/glad.h>
#include <string>

#include "Result.h"
#include "ds/Vector.h"

namespace Gfx {

struct Uniform
{
  enum class Type
  {
    Model,
    Proj,
    View,
    ViewPos,
    Sampler,
    Color,
    AlphaColor,
    MemberId,
    Time,
    FillAmount,
    ADiffuse,
    ASpecular,
    Count,
  };
  static const char* smTypeStrings[Type::Count];
  Type mType;
  GLint mLocation;
};

struct Shader
{
public:
  Result Init(const Ds::Vector<std::string>& paths);
  void Finalize() {};
  void Purge();

  Shader();
  Shader(Shader&& other);
  Shader& operator=(Shader&& other);
  ~Shader();
  Shader(const char* vertexFile, const char* fragmentFile);

  Result Init(const char* vertexFile, const char* fragmentFile);
  GLuint Id() const;
  GLint UniformLocation(Uniform::Type type) const;

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
    const char* shaderSource, int shaderType, unsigned int* shaderId);
  void InitializeUniforms();
};

} // namespace Gfx

#endif
