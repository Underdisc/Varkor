#ifndef gfx_Shader_h
#define gfx_Shader_h

#include <string>

#include "ds/Vector.h"

namespace Gfx {

class Shader
{
public:
  struct InitResult
  {
    bool mSuccess;
    std::string mError;
  };

public:
  Shader();
  Shader(const char* vertexFile, const char* fragmentFile);
  InitResult Init(const char* vertexFile, const char* fragmentFile);
  void Use() const;
  unsigned int Id() const;
  bool Live() const;
  int UniformLocation(const char* name) const;
  void SetInt(const char* name, int value) const;
  void SetFloat(const char* name, float value) const;
  void SetVec3(const char* name, const float* data) const;
  void SetVec4(const char* name, const float* data) const;
  void SetMat4(
    const char* name, const float* data, bool transpose = true) const;
  void SetSampler(const char* name, int textureUnit) const;

private:
  unsigned int mProgram;

  static constexpr int smInvalidLocation = -1;

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
  Shader::IncludeResult HandleIncludes(
    const char* shaderFile, std::string& content);
  InitResult Compile(
    const char* shaderSource, int shaderType, unsigned int* shaderId);
};

} // namespace Gfx

#endif
