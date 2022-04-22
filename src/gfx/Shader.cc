#include <filesystem>
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

int Shader::GetChunkIndex(int lineNumber, const Ds::Vector<SourceChunk>& chunks)
{
  for (int i = 0; i < chunks.Size(); ++i) {
    if (lineNumber < chunks[i].mEndLine) {
      return i;
    }
  }
  std::stringstream error;
  error << "No chunk contains the line number " << lineNumber;
  LogAbort(error.str().c_str());
  return -1;
}

Shader::IncludeResult Shader::HandleIncludes(
  const std::string& file, std::string* content)
{
  // The first source chunk is the file we are currently in.
  IncludeResult result;
  SourceChunk startChunk;
  startChunk.mFile = file;
  startChunk.mExcludedLines = 0;
  startChunk.mStartLine = 1;
  startChunk.mEndLine = GetLineNumber(content->size(), *content) + 1;
  result.mChunks.Push(startChunk);

  // We repeatedly search for include statements until there are none left.
  std::string& text = *content;
  std::regex expression("#include \"([^\"]*)\"");
  std::smatch match;
  while (std::regex_search(text.cbegin(), text.cend(), match, expression)) {
    int includeLine = GetLineNumber(match[0].first - text.begin(), text);
    int chunkIndex = GetChunkIndex(includeLine, result.mChunks);
    SourceChunk& currentChunk = result.mChunks[chunkIndex];

    // Find a full path to the file that is relative to the executable.
    std::string includeFilename;
    std::size_t pathEnd = currentChunk.mFile.find_last_of('/');
    std::string path(currentChunk.mFile.substr(0, pathEnd + 1));
    // First we check relative to the path of the current chunk's file.
    if (std::filesystem::exists(path + match[1].str())) {
      includeFilename = path + match[1].str();
    }
    // Then we check the path relative to the executable.
    else if (std::filesystem::exists(match[1].str())) {
      includeFilename = match[1].str();
    }
    else {
      result.mSuccess = false;
      int chunkLine = currentChunk.mExcludedLines +
        (includeLine - currentChunk.mStartLine) + 1;
      std::stringstream error;
      error << currentChunk.mFile << "(" << chunkLine << "): "
            << "Failed to include \"" << match[1].str() << "\".";
      result.mError = error.str();
      return result;
    }

    // Read the file's content.
    std::ifstream file;
    file.open(includeFilename);
    std::stringstream includeFileStream;
    includeFileStream << file.rdbuf();
    std::string includeContent = includeFileStream.str();
    int includeLineCount = GetLineNumber(includeContent.size(), includeContent);

    // Two new chunks are added for any inclusion. The first chunk is for the
    // code from the included file.
    SourceChunk includeChunk;
    includeChunk.mFile = includeFilename;
    includeChunk.mStartLine = includeLine;
    includeChunk.mEndLine = includeLine + includeLineCount;
    includeChunk.mExcludedLines = 0;
    result.mChunks.Insert(chunkIndex + 1, includeChunk);

    // The other chunk is for the code that comes after the inclusion.
    int addedNewlineCount = includeLineCount - 1;
    SourceChunk splitChunk;
    splitChunk.mFile = currentChunk.mFile;
    splitChunk.mStartLine = includeChunk.mEndLine;
    splitChunk.mEndLine = currentChunk.mEndLine + addedNewlineCount;
    splitChunk.mExcludedLines = currentChunk.mExcludedLines;
    splitChunk.mExcludedLines += includeLine + 1 - currentChunk.mStartLine;
    result.mChunks.Insert(chunkIndex + 2, splitChunk);

    // Modify existing chunks to account for the new inclusion.
    currentChunk.mEndLine = includeLine;
    for (int i = chunkIndex + 3; i < result.mChunks.Size(); ++i) {
      result.mChunks[i].mStartLine += addedNewlineCount;
      result.mChunks[i].mEndLine += addedNewlineCount;
    }

    text.replace(
      match[0].first,
      match[0].second,
      includeContent.begin(),
      includeContent.end());
  }
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
  IncludeResult includeResult = HandleIncludes(shaderFile, &fileContentStr);
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
    int chunkIndex = GetChunkIndex(errorLineNumber, includeResult.mChunks);
    const SourceChunk& chunk = includeResult.mChunks[chunkIndex];
    int chunkLineNumber = (errorLineNumber - chunk.mStartLine) + 1;
    int trueLineNumber = chunk.mExcludedLines + chunkLineNumber;
    error << chunk.mFile << "(" << trueLineNumber << ")";
    mItE = match[0].second;
  }
  error << logStr.substr(mItE - logStr.cbegin());
  return Result(error.str());
}

} // namespace Gfx