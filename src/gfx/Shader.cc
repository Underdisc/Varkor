#include <fstream>
#include <glad/glad.h>
#include <regex>
#include <sstream>

#include "Error.h"
#include "Util/Utility.h"

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

void Shader::SetInt(const char* name, int value) const
{
  int loc = UniformLocation(name);
  Use();
  glUniform1i(loc, value);
}

void Shader::SetFloat(const char* name, float value) const
{
  int loc = UniformLocation(name);
  Use();
  glUniform1f(loc, value);
}

void Shader::SetVec2(const char* name, const float* data) const
{
  int loc = UniformLocation(name);
  Use();
  glUniform2fv(loc, 1, data);
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

int GetLineNumber(size_t until, const std::string& string)
{
  int lineNumber = 1;
  size_t currentChar = string.find('\n', 0);
  while (currentChar < until)
  {
    ++lineNumber;
    currentChar = string.find('\n', currentChar + 1);
  }
  return lineNumber;
}

Shader::IncludeResult Shader::HandleIncludes(
  const char* shaderFile, std::string& content)
{
  // Extract the path of the file.
  std::string fileStr(shaderFile);
  std::size_t pathEnd = fileStr.find_last_of('/');
  std::string path(fileStr.substr(0, pathEnd + 1));

  // The first source chunk is the file we are currently in.
  SourceChunk chunk;
  chunk.mFile = shaderFile;
  chunk.mExcludedLines = 0;
  chunk.mStartLine = 1;
  IncludeResult result;
  result.mChunks.Push(chunk);

  std::regex expression("#include \"([^\"]*)\"");
  std::smatch match;
  while (std::regex_search(content.cbegin(), content.cend(), match, expression))
  {
    // Find the backmost chunk's end line, and use it to find the number of
    // lines excluded from the next chunk for the current file.
    int includeLine = GetLineNumber(match[0].first - content.begin(), content);
    result.mChunks.Top().mEndLine = includeLine;
    chunk.mExcludedLines += includeLine - result.mChunks.Top().mStartLine + 1;

    // Remove the backmost chunk if it doesn't contain content.
    if (result.mChunks.Top().mStartLine == result.mChunks.Top().mEndLine)
    {
      result.mChunks.Pop();
    }

    // Open and read the content of the included file.
    std::string includeFilename(path);
    includeFilename.append(match[1].str());
    std::ifstream file;
    file.open(includeFilename);
    if (!file.is_open())
    {
      result.mSuccess = false;
      std::stringstream error;
      error << shaderFile << "(" << chunk.mExcludedLines << "): "
            << "Failed to include \"" << match[1].str() << "\".";
      result.mError = error.str();
      return result;
    }
    std::stringstream includeFileStream;
    includeFileStream << file.rdbuf();
    std::string includeContent = includeFileStream.str();

    // Handle any includes that show up within the included content and replace
    // the include statement in the content with the included content.
    IncludeResult subResult =
      HandleIncludes(includeFilename.c_str(), includeContent);
    if (!subResult.mSuccess)
    {
      return subResult;
    }
    content.replace(
      match[0].first,
      match[0].second,
      includeContent.begin(),
      includeContent.end() - 1);

    // Take the returned subchunks and offset both their start and end lines
    // by the line the chunk was included on.
    for (SourceChunk& subChunk : subResult.mChunks)
    {
      result.mChunks.Push(subChunk);
      result.mChunks.Top().mStartLine += includeLine - 1;
      result.mChunks.Top().mEndLine += includeLine - 1;
    }

    // Now that we've handled all the sub chunks, we make the current file the
    // backmost chunk again.
    chunk.mStartLine = result.mChunks.Top().mEndLine;
    result.mChunks.Push(chunk);
  }
  result.mChunks.Top().mEndLine = GetLineNumber(content.size(), content);
  result.mSuccess = true;
  return result;
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
    reason << "Failed to open " << shaderFile;
    InitResult result;
    result.mSuccess = false;
    result.mError = reason.str();
    return result;
  }
  std::stringstream fileContentStream;
  fileContentStream << file.rdbuf();
  std::string fileContentStr = fileContentStream.str();

  // Handle all of the include statements within the file content.
  IncludeResult includeResult = HandleIncludes(shaderFile, fileContentStr);
  if (!includeResult.mSuccess)
  {
    InitResult result;
    result.mSuccess = false;
    std::stringstream reason;
    reason << "Failed to compile " << shaderFile << "." << std::endl
           << includeResult.mError;
    result.mError = reason.str();
    return result;
  }

  // Compile the source read from the file.
  *shaderId = glCreateShader(shaderType);
  const char* fileContent = fileContentStr.c_str();
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

  // The shader failed to compile and we need to retrieve the errors.
  const int logLen = 512;
  char log[logLen];
  glGetShaderInfoLog(*shaderId, logLen, NULL, log);
  std::string logStr(log);
  std::stringstream error;
  error << "Failed to compile " << shaderFile << "." << std::endl;

  // We need to modify the retrieved error log so it has proper line numbers.
  std::regex expression("0\\(([0-9]+)\\)");
  std::smatch match;
  std::string::const_iterator mItB = logStr.cbegin();
  std::string::const_iterator mItE = logStr.cbegin();
  while (std::regex_search(mItE, logStr.cend(), match, expression))
  {
    error << logStr.substr(mItE - logStr.cbegin(), match[0].first - mItE);
    int completeLine = std::stoi(match[1].str());
    for (const SourceChunk& chunk : includeResult.mChunks)
    {
      if (completeLine < chunk.mEndLine)
      {
        int chunkLineNumber = (completeLine - chunk.mStartLine) + 1;
        int trueLine = chunk.mExcludedLines + chunkLineNumber;
        error << chunk.mFile << "(" << trueLine << ")";
        break;
      }
    }
    mItB = match[0].first;
    mItE = match[0].second;
  }
  error << logStr.substr(mItE - logStr.cbegin());

  InitResult result;
  result.mSuccess = false;
  result.mError = error.str();
  return result;
}

} // namespace Gfx