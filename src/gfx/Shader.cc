#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <regex>
#include <sstream>
#include <string.h>
#include <utility>

#include "AssetLibrary.h"
#include "Error.h"
#include "Shader.h"
#include "util/Utility.h"

namespace Gfx {

const char* Uniform::smTypeStrings[] = {
  "uModel",
  "uTexture",
  "uColor",
  "uAlphaColor",
  "uMemberId",
  "uFillAmount",
  "uSkyboxSampler",
  "uMateial.mDiffuse",
  "uMaterial.mSpecular"};

bool Shader::smLogMissingUniforms = false;
const char* Shader::smVersionHeader = "#version 330 core\n";
const char* Shader::smShaderTypeStrings[] = {"vertex", "fragment"};
const GLenum Shader::smShaderTypes[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
const int Shader::smShaderTypeCount = sizeof(smShaderTypes) / sizeof(GLenum);

void Shader::InitInfo::Prep(const std::string& file)
{
  mFiles.Push(file);
}

void Shader::InitInfo::Serialize(Vlk::Value& val) const
{
  Vlk::Value& filesVal = val("Files")[mFiles.Size()];
  for (int i = 0; i < mFiles.Size(); ++i) {
    filesVal[i] = mFiles[i];
  }
}

void Shader::InitInfo::Deserialize(const Vlk::Explorer& ex)
{
  Vlk::Explorer filesEx = ex("Files");
  for (int i = 0; i < filesEx.Size(); ++i) {
    mFiles.Push(filesEx[i].As<std::string>());
  }
}

void Shader::Purge()
{
  glDeleteProgram(mId);
  mId = 0;
  mUniforms.Clear();
}

Shader::Shader(): mId(0) {}

Shader::Shader(Shader&& other)
{
  *this = std::forward<Shader>(other);
}

Shader& Shader::operator=(Shader&& other)
{
  mId = other.mId;
  other.mId = 0;
  return *this;
}

Shader::~Shader()
{
  Purge();
}

Result Shader::Init(const InitInfo& info)
{
  Purge();
  Result result = CreateProgram(info.mFiles);
  if (!result.Success()) {
    std::stringstream error;
    error << "Failed shader program creation.\n" << result.mError;
    return Result(error.str());
  }
  InitializeUniforms();
  glFinish();
  return result;
}

Result Shader::Init(const std::string& file)
{
  InitInfo info;
  info.Prep(file.c_str());
  return Init(info);
}

Result Shader::Init(
  const std::string& vertexFile, const std::string& fragmentFile)
{
  InitInfo info;
  info.Prep(vertexFile.c_str(), fragmentFile.c_str());
  return Init(info);
}

GLuint Shader::Id() const
{
  return mId;
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

GLint Shader::UniformLocation(const char* name) const
{
  GLint location = glGetUniformLocation(mId, name);
  if (location == -1 && smLogMissingUniforms) {
    std::stringstream error;
    error << "Shader " << mId << " has no \"" << name << "\" uniform.";
    LogError(error.str().c_str());
  }
  return location;
}

void Shader::Use() const
{
  glUseProgram(mId);
}

void Shader::SetUniform(const char* name, float value) const
{
  GLint location = UniformLocation(name);
  glUniform1f(location, value);
}

void Shader::SetUniform(const char* name, int value) const
{
  GLint location = UniformLocation(name);
  glUniform1i(location, value);
}

void Shader::SetUniform(const char* name, const Vec3& value) const
{
  GLint location = UniformLocation(name);
  glUniform3fv(location, 1, value.CData());
}

void Shader::SetUniform(const char* name, const Vec4& value) const
{
  GLint location = UniformLocation(name);
  glUniform4fv(location, 1, value.CData());
}

void Shader::SetUniform(const char* name, const Mat4& value) const
{
  GLint location = UniformLocation(name);
  glUniformMatrix4fv(location, 1, true, value.CData());
}

void Shader::InitializeUniforms()
{
  for (int i = 0; i < (int)Uniform::Type::Count; ++i) {
    Uniform::Type uniformType = (Uniform::Type)i;
    const char* typeString = Uniform::smTypeStrings[i];
    int location = glGetUniformLocation(mId, typeString);
    if (location != smInvalidLocation) {
      Uniform newUniform;
      newUniform.mType = uniformType;
      newUniform.mLocation = location;
      mUniforms.Push(newUniform);
    }
  }

  auto tryBindUniformBlock = [this](const char* name, GLuint binding)
  {
    GLuint index = glGetUniformBlockIndex(mId, name);
    if (index != GL_INVALID_INDEX) {
      glUniformBlockBinding(mId, index, binding);
    }
  };
  tryBindUniformBlock("Universal", 0);
  tryBindUniformBlock("Lights", 1);
}

ValueResult<std::string> Shader::GetFileContent(const std::string& filename)
{
  std::ifstream file;
  file.open(filename);
  if (!file.is_open()) {
    std::stringstream reason;
    reason << "Failed to open \"" << filename << "\"";
    return ValueResult<std::string>(reason.str(), "");
  }
  std::stringstream fileContentStream;
  fileContentStream << file.rdbuf();
  return ValueResult<std::string>(fileContentStream.str());
}

int Shader::GetLineNumber(size_t until, const std::string& string)
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

Result Shader::HandleIncludes(
  std::string* content, Ds::Vector<SourceChunk>* chunks)
{
  // We repeatedly search for include statements until there are none left.
  std::string& text = *content;
  std::regex expression("#include \"([^\"]*)\"");
  std::smatch match;
  while (std::regex_search(text.cbegin(), text.cend(), match, expression)) {
    int includeLine = GetLineNumber(match[0].first - text.begin(), text);
    int currentChunkIndex = GetChunkIndex(includeLine, *chunks);
    SourceChunk currentChunk = (*chunks)[currentChunkIndex];

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
      int chunkLine = currentChunk.mExcludedLines +
        (includeLine - currentChunk.mStartLine) + 1;
      std::stringstream error;
      error << currentChunk.mFile << "(" << chunkLine << ") : "
            << "Failed to include \"" << match[1].str() << "\"";
      return Result(error.str());
    }

    // Ignore the inclusion if the file has already been included.
    bool includedGuarded = false;
    for (const SourceChunk& chunk : *chunks) {
      if (includeFilename == chunk.mFile) {
        text.replace(match[0].first, match[0].second, "");
        includedGuarded = true;
        break;
      }
    }
    if (includedGuarded) {
      continue;
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
    chunks->Insert(currentChunkIndex + 1, includeChunk);

    // The other chunk is for the code that comes after the inclusion.
    int addedNewlineCount = includeLineCount - 1;
    SourceChunk splitChunk;
    splitChunk.mFile = currentChunk.mFile;
    splitChunk.mStartLine = includeChunk.mEndLine;
    splitChunk.mEndLine = currentChunk.mEndLine + addedNewlineCount;
    splitChunk.mExcludedLines = currentChunk.mExcludedLines;
    splitChunk.mExcludedLines += includeLine + 1 - currentChunk.mStartLine;
    chunks->Insert(currentChunkIndex + 2, splitChunk);

    // Modify existing chunks to account for the new inclusion.
    (*chunks)[currentChunkIndex].mEndLine = includeLine;
    for (int i = currentChunkIndex + 3; i < (*chunks).Size(); ++i) {
      (*chunks)[i].mStartLine += addedNewlineCount;
      (*chunks)[i].mEndLine += addedNewlineCount;
    }

    text.replace(
      match[0].first,
      match[0].second,
      includeContent.begin(),
      includeContent.end());
  }
  return Result();
}

Result Shader::Compile(CompileInfo* compileInfo, GLuint shaderId)
{
  // Handle all of the include statements within the file content and prepend
  // the version header.
  SourceChunk firstChunk;
  firstChunk.mFile = compileInfo->mFile;
  firstChunk.mStartLine = 1;
  firstChunk.mEndLine =
    GetLineNumber(compileInfo->mSource.size(), compileInfo->mSource) + 1;
  firstChunk.mExcludedLines = compileInfo->mExcludedLines;
  Ds::Vector<SourceChunk> chunks;
  chunks.Push(std::move(firstChunk));
  Result result = HandleIncludes(&compileInfo->mSource, &chunks);
  if (!result.Success()) {
    return Result(result.mError);
  }
  compileInfo->mSource = smVersionHeader + compileInfo->mSource;

  // Compile the given preprocessed source.
  const char* sourceCStr = compileInfo->mSource.c_str();
  glShaderSource(shaderId, 1, &sourceCStr, NULL);
  glCompileShader(shaderId);
  int success;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if (success) {
    return Result();
  }

  // The shader failed to compile. We need to retrieve the errors.
  const int bufferSize = 512;
  char log[bufferSize];
  glGetShaderInfoLog(shaderId, bufferSize, NULL, log);
  size_t logLength = strlen(log);
  // Subtracting 1 removes the last newline character.
  std::string logStr(log, logLength - 1);

  // The log string must be modified to make errors reference the correct files
  // and line numbers.
  std::stringstream error;
  std::regex expression("0\\(([0-9]+)\\)");
  std::smatch match;
  std::string::const_iterator mItE = logStr.cbegin();
  while (std::regex_search(mItE, logStr.cend(), match, expression)) {
    error << logStr.substr(mItE - logStr.cbegin(), match[0].first - mItE);
    // Subtracting 1 ignores the version header line.
    int errorLineNumber = std::stoi(match[1].str()) - 1;
    int chunkIndex = GetChunkIndex(errorLineNumber, chunks);
    const SourceChunk& chunk = chunks[chunkIndex];
    int chunkLineNumber = (errorLineNumber - chunk.mStartLine) + 1;
    int trueLineNumber = chunk.mExcludedLines + chunkLineNumber;
    error << chunk.mFile << "(" << trueLineNumber << ")";
    mItE = match[0].second;
  }
  error << logStr.substr(mItE - logStr.cbegin());
  return Result(error.str());
}

Result Shader::CreateProgram(Ds::Vector<CompileInfo>* allCompileInfo)
{
  // Compile all of the shaders.
  Ds::Vector<GLuint> shaderIds;
  for (CompileInfo& compileInfo : *allCompileInfo) {
    GLuint newShaderId = glCreateShader(compileInfo.mShaderType);
    Result compileResult = Compile(&compileInfo, newShaderId);
    shaderIds.Push(newShaderId);

    // We need to delete all of the created shaders if a compilation fails.
    if (!compileResult.Success()) {
      for (GLuint shaderId : shaderIds) {
        glDeleteShader(shaderId);
      }
      std::stringstream error;
      error << "Failed to compile \"" << compileInfo.mFile << "\".\n"
            << compileResult.mError;
      return Result(error.str());
    }
  }

  // Create the shader program.
  mId = glCreateProgram();
  for (GLuint shaderId : shaderIds) {
    glAttachShader(mId, shaderId);
  }
  glLinkProgram(mId);
  for (GLuint shaderId : shaderIds) {
    glDeleteShader(shaderId);
  }

  // Ensure that the link was successful.
  GLint linkStatus;
  glGetProgramiv(mId, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    GLint length;
    glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &length);
    char* errorLog = new char[length];
    // Subtracting one removes a newline.
    glGetProgramInfoLog(mId, length - 1, NULL, errorLog);
    std::stringstream error;
    error << "Failed to link.\n" << errorLog;
    delete errorLog;
    glDeleteProgram(mId);
    mId = 0;
    return Result(error.str());
  }
  return Result();
}

ValueResult<Ds::Vector<Shader::CompileInfo>> Shader::CollectCompileInfo(
  const std::string& file)
{
  // Resolve the file.
  ValueResult<std::string> resolutionResult = AssLib::ResolveResourcePath(file);
  if (!resolutionResult.Success()) {
    return Result(resolutionResult.mError);
  }
  const std::string& truePath = resolutionResult.mValue;

  // Get the file content.
  ValueResult<std::string> fileConentResult = GetFileContent(truePath);
  if (!fileConentResult.Success()) {
    return Result(fileConentResult.mError);
  }
  const std::string& content = fileConentResult.mValue;

  // Used to track the headers for each shader in the file.
  struct HeaderData
  {
    // The range of the header [mStart, mEnd).
    size_t mStart;
    size_t mEnd;
    GLenum mShaderType;
  };
  Ds::Vector<HeaderData> allHeaderData;

  // Define the regular expressing for finding the type headers.
  std::string regexString = "#type (";
  regexString += smShaderTypeStrings[0];
  for (int i = 1; i < smShaderTypeCount; ++i) {
    regexString += "|";
    regexString += smShaderTypeStrings[i];
  }
  regexString += "|[^\n]*)";

  // Find all of the unique type headers.
  std::regex expression(regexString.c_str());
  std::smatch match;
  std::string::const_iterator searchStart = content.cbegin();
  while (std::regex_search(searchStart, content.cend(), match, expression)) {
    HeaderData newHeaderData;
    newHeaderData.mStart = match[0].first - content.cbegin();
    newHeaderData.mEnd = match[0].second - content.cbegin();

    // Find the header type.
    constexpr GLenum invalidShaderType = 0;
    newHeaderData.mShaderType = invalidShaderType;
    for (int i = 0; i < smShaderTypeCount; ++i) {
      if (match[1].str() == smShaderTypeStrings[i]) {
        newHeaderData.mShaderType = smShaderTypes[i];
        break;
      }
    }

    // Ensure that the header is valid.
    if (newHeaderData.mShaderType == invalidShaderType) {
      std::stringstream error;
      error << file << "(" << GetLineNumber(newHeaderData.mStart, content)
            << ") : Invalid shader type \"" << match[1].str() << "\".";
      return Result(error.str());
    }
    allHeaderData.Push(newHeaderData);
    searchStart = match[0].second;
  }

  // Create the CompileInfo for the source under each type header.
  Ds::Vector<CompileInfo> collection;
  for (int i = 0; i < allHeaderData.Size(); ++i) {
    const HeaderData& headerData = allHeaderData[i];
    size_t sourceBegin = headerData.mEnd;
    size_t sourceSize;
    if (i == allHeaderData.Size() - 1) {
      size_t fileSize = content.cend() - content.cbegin();
      sourceSize = fileSize - sourceBegin;
    }
    else {
      const HeaderData& nextHeaderData = allHeaderData[i + 1];
      sourceSize = nextHeaderData.mStart - headerData.mEnd;
    }
    CompileInfo newCompileInfo;
    newCompileInfo.mSource = content.substr(sourceBegin, sourceSize);
    newCompileInfo.mFile = file;
    newCompileInfo.mExcludedLines = GetLineNumber(sourceBegin, content) - 1;
    newCompileInfo.mShaderType = headerData.mShaderType;
    collection.Push(std::move(newCompileInfo));
  }
  return ValueResult<Ds::Vector<CompileInfo>>(std::move(collection));
}

Result Shader::CreateProgram(const Ds::Vector<std::string>& files)
{
  // Collect of the CompileInfo from the provided files.
  Ds::Vector<CompileInfo> allCompileInfo;
  for (const std::string& file : files) {
    ValueResult<Ds::Vector<CompileInfo>> result = CollectCompileInfo(file);
    if (!result.Success()) {
      return result;
    }
    for (CompileInfo& compileInfo : result.mValue) {
      allCompileInfo.Push(std::move(compileInfo));
    }
  }

  // Ensure that there aren't multiple versions of the same shader type.
  for (int i = 0; i < smShaderTypeCount; ++i) {
    Ds::Vector<int> infoIndices;
    for (int j = 0; j < allCompileInfo.Size(); ++j) {
      if (allCompileInfo[j].mShaderType == smShaderTypes[i]) {
        infoIndices.Push(j);
      }
    }
    if (infoIndices.Size() <= 1) {
      continue;
    }
    std::stringstream error;
    bool firstIndex = true;
    for (int j = 0; j < infoIndices.Size(); ++j) {
      if (j > 0) {
        error << "\n";
      }
      const CompileInfo& info = allCompileInfo[infoIndices[j]];
      error << info.mFile << "(" << info.mExcludedLines + 1 << ") : Multiple \""
            << smShaderTypeStrings[i] << "\" shaders";
    }
    return Result(error.str());
  }
  return CreateProgram(&allCompileInfo);
}

} // namespace Gfx