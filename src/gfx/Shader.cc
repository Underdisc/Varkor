#include <fstream>
#include <glad/glad.h>
#include <regex>
#include <sstream>
#include <string.h>

#include "Error.h"
#include "Shader.h"
#include "util/Utility.h"

namespace Gfx {

const char* Uniform::smTypeStrings[Uniform::Type::Count] = {
  "uModel",
  "uProj",
  "uView",
  "uViewPos",
  "uTexture",
  "uColor",
  "uAlphaColor",
  "uMemberId",
  "uTime",
  "uFillAmount",
  "uMateial.mDiffuse",
  "uMaterial.mSpecular"};
bool Shader::smLogMissingUniforms = false;

Result Shader::Init(const Ds::Vector<std::string>& paths)
{
  return Init(paths[0].c_str(), paths[1].c_str());
}

void Shader::Purge()
{
  glDeleteProgram(mProgram);
  mProgram = 0;
}

Shader::Shader(): mProgram(0) {}

Shader::Shader(Shader&& other)
{
  *this = Util::Forward(other);
}

Shader& Shader::operator=(Shader&& other)
{
  mProgram = other.mProgram;
  other.mProgram = 0;
  return *this;
}

Shader::~Shader()
{
  Purge();
}

Shader::Shader(const char* vertexFile, const char* fragmentFile): mProgram(0)
{
  Result result = Init(vertexFile, fragmentFile);
  LogAbortIf(!result.Success(), result.mError.c_str());
}

Result Shader::Init(const char* vertexFile, const char* fragmentFile)
{
  Purge();

  // Compile the provided shader files.
  unsigned int vertexId, fragmentId;
  Result result = Compile(vertexFile, GL_VERTEX_SHADER, &vertexId);
  if (!result.Success()) {
    return result;
  }
  result = Compile(fragmentFile, GL_FRAGMENT_SHADER, &fragmentId);
  if (!result.Success()) {
    return result;
  }

  // Link the compiled shaders.
  mProgram = glCreateProgram();
  glAttachShader(mProgram, vertexId);
  glAttachShader(mProgram, fragmentId);
  glLinkProgram(mProgram);
  glDeleteShader(vertexId);
  glDeleteShader(fragmentId);
  int linked;
  glGetProgramiv(mProgram, GL_LINK_STATUS, &linked);
  if (!linked) {
    const int errorLogLen = 512;
    char errorLog[errorLogLen];
    glGetProgramInfoLog(mProgram, errorLogLen, NULL, errorLog);
    std::stringstream reason;
    reason << " shader files " << vertexFile << " and " << fragmentFile
           << " failed to link." << std::endl
           << errorLog;
    result.mError = reason.str();
    mProgram = 0;
  }
  InitializeUniforms();
  glFinish();
  return result;
}

GLuint Shader::Id() const
{
  return mProgram;
}

GLint Shader::UniformLocation(Uniform::Type type) const
{
  for (const Uniform& uniform : mUniforms) {
    if (uniform.mType == type) {
      return uniform.mLocation;
    }
  }
  if (smLogMissingUniforms) {
    const char* typeString = Uniform::smTypeStrings[(int)type];
    std::stringstream error;
    error << "Shader does not contain the " << typeString << " uniform.";
    LogError(error.str().c_str());
  }
  return smInvalidLocation;
}

void Shader::InitializeUniforms()
{
  auto tryAddUniform = [this](Uniform::Type type)
  {
    const char* typeString = Uniform::smTypeStrings[(int)type];
    int location = glGetUniformLocation(mProgram, typeString);
    if (location != smInvalidLocation) {
      Uniform newUniform;
      newUniform.mType = type;
      newUniform.mLocation = location;
      mUniforms.Push(newUniform);
    }
  };

  tryAddUniform(Uniform::Type::Model);
  tryAddUniform(Uniform::Type::Proj);
  tryAddUniform(Uniform::Type::View);
  tryAddUniform(Uniform::Type::ViewPos);
  tryAddUniform(Uniform::Type::Sampler);
  tryAddUniform(Uniform::Type::Color);
  tryAddUniform(Uniform::Type::AlphaColor);
  tryAddUniform(Uniform::Type::MemberId);
  tryAddUniform(Uniform::Type::Time);
  tryAddUniform(Uniform::Type::FillAmount);
  tryAddUniform(Uniform::Type::ADiffuse);
  tryAddUniform(Uniform::Type::ASpecular);
}

int GetLineNumber(size_t until, const std::string& string)
{
  int lineNumber = 1;
  size_t currentChar = string.find('\n', 0);
  while (currentChar < until) {
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
  while (
    std::regex_search(content.cbegin(), content.cend(), match, expression)) {
    // Find the backmost chunk's end line, and use it to find the number of
    // lines excluded from the next chunk for the current file.
    int includeLine = GetLineNumber(match[0].first - content.begin(), content);
    result.mChunks.Top().mEndLine = includeLine;
    chunk.mExcludedLines += includeLine - result.mChunks.Top().mStartLine + 1;

    // Remove the backmost chunk if it doesn't contain content.
    if (result.mChunks.Top().mStartLine == result.mChunks.Top().mEndLine) {
      result.mChunks.Pop();
    }

    // Open and read the content of the included file.
    std::string includeFilename(path);
    includeFilename.append(match[1].str());
    std::ifstream file;
    file.open(includeFilename);
    if (!file.is_open()) {
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
    if (!subResult.mSuccess) {
      return subResult;
    }
    content.replace(
      match[0].first,
      match[0].second,
      includeContent.begin(),
      includeContent.end() - 1);

    // Take the returned subchunks and offset both their start and end lines
    // by the line the chunk was included on.
    for (SourceChunk& subChunk : subResult.mChunks) {
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

Result Shader::Compile(
  const char* shaderFile, int shaderType, unsigned int* shaderId)
{
  // Read the content of the provided file.
  std::ifstream file;
  file.open(shaderFile);
  if (!file.is_open()) {
    std::stringstream reason;
    reason << "Failed to open " << shaderFile;
    return Result(reason.str());
  }
  std::stringstream fileContentStream;
  fileContentStream << file.rdbuf();
  std::string fileContentStr = fileContentStream.str();

  // Handle all of the include statements within the file content and prepend
  // the version header.
  IncludeResult includeResult = HandleIncludes(shaderFile, fileContentStr);
  if (!includeResult.mSuccess) {
    std::stringstream reason;
    reason << "Failed to compile " << shaderFile << "." << std::endl
           << includeResult.mError;
    return Result(reason.str());
  }
  fileContentStr = smVersionHeader + fileContentStr;

  // Compile the source read from the file.
  *shaderId = glCreateShader(shaderType);
  const char* fileContent = fileContentStr.c_str();
  glShaderSource(*shaderId, 1, &fileContent, NULL);
  glCompileShader(*shaderId);
  int success;
  glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &success);
  if (success) {
    return Result();
  }

  // The shader failed to compile and we need to retrieve the errors.
  const int bufferSize = 512;
  char log[bufferSize];
  glGetShaderInfoLog(*shaderId, bufferSize, NULL, log);
  size_t logLength = strlen(log);
  // Subtracting 1 removes the last newline character.
  std::string logStr(log, logLength - 1);
  std::stringstream error;
  error << "Failed to compile " << shaderFile << ".\n";

  // We need to modify the retrieved error log so it has proper line numbers.
  std::regex expression("0\\(([0-9]+)\\)");
  std::smatch match;
  std::string::const_iterator mItE = logStr.cbegin();
  while (std::regex_search(mItE, logStr.cend(), match, expression)) {
    error << logStr.substr(mItE - logStr.cbegin(), match[0].first - mItE);
    // Subtracting 1 ignores the version header line.
    int errorLineNumber = std::stoi(match[1].str()) - 1;
    for (const SourceChunk& chunk : includeResult.mChunks) {
      if (errorLineNumber < chunk.mEndLine) {
        int chunkLineNumber = (errorLineNumber - chunk.mStartLine) + 1;
        int trueLineNumber = chunk.mExcludedLines + chunkLineNumber;
        error << chunk.mFile << "(" << trueLineNumber << ")";
        break;
      }
    }
    mItE = match[0].second;
  }
  error << logStr.substr(mItE - logStr.cbegin());
  return Result(error.str());
}

} // namespace Gfx