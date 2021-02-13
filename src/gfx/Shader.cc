#include <glad/glad.h>

#include <fstream>
#include <sstream>

#include "Error.h"

#include "Shader.h"

namespace Gfx {

Shader::Shader(): mProgram(0) {}

Shader::Shader(const char* vertexFile, const char* fragmentFile): mProgram(0)
{
  InitResult result = Init(vertexFile, fragmentFile);
  if (!result.mSuccess)
  {
    LogAbort(result.mError.c_str());
  }
}

Shader::InitResult Shader::Init(
  const char* vertexFile, const char* fragmentFile)
{
  // Delete the current shader program if it exists.
  if (mProgram != 0)
  {
    glDeleteProgram(mProgram);
    mProgram = 0;
  }

  // Compile the provided shader files.
  unsigned int vertId, fragId;
  InitResult result = Compile(vertexFile, GL_VERTEX_SHADER, &vertId);
  if (!result.mSuccess)
  {
    return result;
  }
  result = Compile(fragmentFile, GL_FRAGMENT_SHADER, &fragId);
  if (!result.mSuccess)
  {
    return result;
  }

  // Link the compiled shaders.
  mProgram = glCreateProgram();
  glAttachShader(mProgram, vertId);
  glAttachShader(mProgram, fragId);
  glLinkProgram(mProgram);
  glDeleteShader(vertId);
  glDeleteShader(fragId);
  int linked;
  glGetProgramiv(mProgram, GL_LINK_STATUS, &linked);
  if (!linked)
  {
    const int errorLogLen = 512;
    char errorLog[errorLogLen];
    glGetProgramInfoLog(mProgram, errorLogLen, NULL, errorLog);
    std::stringstream reason;
    reason << " shader files " << vertexFile << " and " << fragmentFile
           << " failed to link." << std::endl
           << errorLog;
    result.mSuccess = false;
    result.mError = reason.str();
    mProgram = 0;
  }
  return result;
}

void Shader::Use() const
{
  glUseProgram(mProgram);
}

unsigned int Shader::Id() const
{
  return mProgram;
}

bool Shader::Live() const
{
  return mProgram != 0;
}

int Shader::UniformLocation(const char* name) const
{
  int loc = glGetUniformLocation(mProgram, name);
  bool valid = loc != smInvalidLocation;
  if (!valid)
  {
    std::stringstream reason;
    reason << name << " uniform not found";
    LogError(reason.str().c_str());
  }
  return loc;
}

void Shader::SetFloat(const char* name, float value) const
{
  int loc = UniformLocation(name);
  Use();
  glUniform1f(loc, value);
}

void Shader::SetVec3(const char* name, const float* data) const
{
  int loc = UniformLocation(name);
  Use();
  glUniform3fv(loc, 1, data);
}

void Shader::SetVec4(const char* name, const float* data) const
{
  int loc = UniformLocation(name);
  Use();
  glUniform4fv(loc, 1, data);
}

void Shader::SetMat4(const char* name, const float* data, bool transpose) const
{
  int loc = UniformLocation(name);
  Use();
  glUniformMatrix4fv(loc, 1, transpose, data);
}

void Shader::SetSampler(const char* name, int textureUnit) const
{
  int loc = UniformLocation(name);
  Use();
  glUniform1i(loc, textureUnit);
}

Shader::InitResult Shader::Compile(
  const char* shaderFile, int shaderType, unsigned int* shaderId)
{
  // Read the content of the provided file.
  std::ifstream file;
  file.open(shaderFile);
  if (!file.is_open())
  {
    std::stringstream reason;
    reason << "shader file " << shaderFile << " does not exist.";
    InitResult result;
    result.mSuccess = false;
    result.mError = reason.str();
    return result;
  }
  std::stringstream fileContentStream;
  fileContentStream << file.rdbuf();
  std::string fileContentStr = fileContentStream.str();
  const char* fileContent = fileContentStr.c_str();

  // Compile the source read from the file.
  *shaderId = glCreateShader(shaderType);
  glShaderSource(*shaderId, 1, &fileContent, NULL);
  glCompileShader(*shaderId);
  int success;
  glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &success);
  if (success)
  {
    InitResult result;
    result.mSuccess = true;
    return result;
  }

  // The shader failed to compile and we retrieve the error here.
  const int logLen = 512;
  char log[logLen];
  glGetShaderInfoLog(*shaderId, logLen, NULL, log);
  std::stringstream error;
  if (shaderType == GL_VERTEX_SHADER)
  {
    error << "vertex";
  } else if (shaderType == GL_FRAGMENT_SHADER)
  {
    error << "fragment";
  }
  error << " shader file " << shaderFile << " failed to compile" << std::endl
        << log;
  InitResult result;
  result.mSuccess = false;
  result.mError = error.str();
  return result;
}

} // namespace Gfx