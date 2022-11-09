#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <regex>
#include <sstream>
#include <string>
#include <utility>

#include "Error.h"
#include "Shader.h"
#include "editor/Utility.h"
#include "rsl/Library.h"
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

Shader::Shader(): mId(0) {}

Shader::Shader(Shader&& other)
{
  *this = std::move(other);
}

Shader& Shader::operator=(Shader&& other)
{
  mId = other.mId;
  mUniforms = std::move(other.mUniforms);
  other.mId = 0;
  return *this;
}

Shader::~Shader()
{
  glDeleteProgram(mId);
  mId = 0;
  mUniforms.Clear();
}

Shader::SubType Shader::GetSubType(const std::string& subTypeString)
{
  for (int i = 0; i < (int)SubType::Count; ++i) {
    if (subTypeString == smSubTypeStrings[i]) {
      return (SubType)i;
    }
  }
  return SubType::Invalid;
}

int Shader::CompileInfo::GetChunkIndex(int lineNumber) const
{
  for (int i = 0; i < mChunks.Size(); ++i) {
    bool afterStart = mChunks[i].mStartLine <= lineNumber;
    if (afterStart && lineNumber < mChunks[i].mEndLine) {
      return i;
    }
  }
  std::stringstream error;
  error << "Line number " << lineNumber << " not found in chunks.";
  LogAbort(error.str().c_str());
  return -1;
}

void Shader::EditConfig(Vlk::Value* configValP)
{
  ImGuiTableFlags flags = ImGuiTableFlags_Resizable |
    ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV;
  if (!ImGui::BeginTable("Files", 2, flags)) {
    return;
  }
  ImGui::TableSetupColumn("File");
  ImGui::TableSetupColumn("");

  ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
  ImGui::TableSetColumnIndex(0);
  ImGui::TableHeader(ImGui::TableGetColumnName(0));

  ImGui::TableSetColumnIndex(1);
  Vlk::Value& configVal = *configValP;
  Vlk::Value& filesVal = configVal("Files");
  filesVal.EnsureType(Vlk::Value::Type::ValueArray);
  if (ImGui::Button("+", ImVec2(-1.0f, 0.0f))) {
    filesVal.PushValue("");
  }
  ImGui::SameLine();
  ImGui::PushID(1);
  ImGui::TableHeader("");
  ImGui::PopID();

  for (int i = 0; i < filesVal.Size(); ++i) {
    ImGui::TableNextRow();
    // A text box that files can be typed into.
    ImGui::TableNextColumn();
    Vlk::Value& fileVal = filesVal[i];
    std::string file = fileVal.As<std::string>("");
    ImGui::PushID(&fileVal);
    // todo: drag and drop.
    Editor::InputText("File", &file);
    ImGui::PopID();
    fileVal = file;

    // A button for removing the current file.
    ImGui::TableNextColumn();
    ImGui::PushID(&filesVal[i]);
    if (ImGui::Button("-", ImVec2(-1.0f, 0.0f))) {
      filesVal.RemoveValue(i);
      --i;
    }
    ImGui::PopID();
  }
  ImGui::EndTable();
}

Result Shader::Init(const Vlk::Explorer& configEx)
{
  // Get the names of the files containing shader source.
  Vlk::Explorer filesEx = configEx("Files");
  if (!filesEx.Valid(Vlk::Value::Type::ValueArray)) {
    return Result("Missing :Files: ValueArray.");
  }
  if (filesEx.Size() == 0) {
    return Result("No Values in :Files:.");
  }
  Ds::Vector<std::string> files;
  for (int i = 0; i < filesEx.Size(); ++i) {
    Vlk::Explorer fileEx = filesEx[i];
    if (!fileEx.Valid(Vlk::Value::Type::TrueValue)) {
      return Result(fileEx.Path() + " must be a TrueValue.");
    }
    files.Push(filesEx[i].As<std::string>());
  }
  return Init(files);
}

Result Shader::Init(const Ds::Vector<std::string>& files)
{
  // Get the CompileInfo contained in each of the files.
  Result result;
  Ds::Vector<CompileInfo> allCompileInfo;
  for (const std::string& file : files) {
    VResult<Ds::Vector<CompileInfo>> collectResult = CollectCompileInfo(file);
    if (!collectResult.Success()) {
      if (!result.mError.empty()) {
        result.mError += '\n';
      }
      result.mError += collectResult.mError;
    }
    auto& collection = collectResult.mValue;
    for (int i = 0; i < collection.Size(); ++i) {
      allCompileInfo.Push(std::move(collection[i]));
    }
  }
  if (!result.Success()) {
    return result;
  }
  return Init(allCompileInfo);
}

Result Shader::Init(const Ds::Vector<CompileInfo>& allCompileInfo)
{
  this->~Shader();
  Result result = CreateProgram(allCompileInfo);
  if (!result.Success()) {
    return result;
  }
  InitializeUniforms();
  return result;
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
  tryBindUniformBlock("Shadow", 2);
}

Result Shader::CompileSubShader(
  const CompileInfo& compileInfo, GLuint subShaderId)
{
  // Add the version header and compile the subshader source.
  std::string source = smVersionHeader + compileInfo.mSource;
  const char* sourceCStr = source.c_str();
  glShaderSource(subShaderId, 1, &sourceCStr, NULL);
  glCompileShader(subShaderId);
  int success;
  glGetShaderiv(subShaderId, GL_COMPILE_STATUS, &success);
  if (success) {
    return Result();
  }

  // The shader failed to compile. We need to retrieve the errors.
  const int bufferSize = 512;
  char log[bufferSize];
  glGetShaderInfoLog(subShaderId, bufferSize, NULL, log);
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
    int chunkIndex = compileInfo.GetChunkIndex(errorLineNumber);
    const SourceChunk& chunk = compileInfo.mChunks[chunkIndex];
    int chunkLineNumber = (errorLineNumber - chunk.mStartLine) + 1;
    int trueLineNumber = chunk.mExcludedLines + chunkLineNumber;
    error << chunk.mFile << "(" << trueLineNumber << ")";
    mItE = match[0].second;
  }
  error << logStr.substr(mItE - logStr.cbegin());
  return Result(error.str());
}

Result Shader::CreateProgram(const Ds::Vector<CompileInfo>& allCompileInfo)
{
  // Ensure that there aren't multiple versions of the same subtype.
  Result result;
  for (int i = 0; i < (int)SubType::Count; ++i) {
    Ds::Vector<const CompileInfo*> infoPointers;
    for (const CompileInfo& info : allCompileInfo) {
      if (info.mSubType == (SubType)i) {
        infoPointers.Push(&info);
      }
    }
    if (infoPointers.Size() <= 1) {
      continue;
    }
    for (int j = 0; j < infoPointers.Size(); ++j) {
      const SourceChunk& firstChunk = infoPointers[j]->mChunks[0];
      result.mError += firstChunk.mFile + "(" +
        std::to_string(firstChunk.mExcludedLines + 1) + ") : Multiple \"" +
        smSubTypeStrings[i] + "\" subshaders.\n";
    }
  }
  if (!result.Success()) {
    result.mError.pop_back();
    return result;
  }

  // Attempt to compile all of the subshaders.
  Ds::Vector<GLuint> subShaderIds;
  for (const CompileInfo& compileInfo : allCompileInfo) {
    GLenum newSubType = smGlSubTypes[(int)compileInfo.mSubType];
    GLuint newSubShaderId = glCreateShader(newSubType);
    Result compileResult = CompileSubShader(compileInfo, newSubShaderId);
    if (!compileResult.Success()) {
      result.mError += compileResult.mError + '\n';
    }
    subShaderIds.Push(newSubShaderId);
  }
  if (!result.Success()) {
    for (GLuint subShaderId : subShaderIds) {
      glDeleteShader(subShaderId);
    }
    result.mError.pop_back();
    return result;
  }

  // Create the shader program.
  mId = glCreateProgram();
  for (GLuint subShaderId : subShaderIds) {
    glAttachShader(mId, subShaderId);
  }
  glLinkProgram(mId);
  for (GLuint subShaderId : subShaderIds) {
    glDeleteShader(subShaderId);
  }

  // Ensure that the link was successful.
  GLint linkStatus;
  glGetProgramiv(mId, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    GLint length;
    glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &length);
    char* errorLog = alloc char[length];
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

VResult<std::string> Shader::GetFileContent(const std::string& file)
{
  VResult<std::string> resolutionResult = Rsl::ResolveResPath(file);
  if (!resolutionResult.Success()) {
    return resolutionResult;
  }
  std::ifstream fileStream;
  fileStream.open(resolutionResult.mValue);
  if (!fileStream.is_open()) {
    return Result("Failed to open \"" + file + "\".");
  }
  std::stringstream fileContentStream;
  fileContentStream << fileStream.rdbuf();
  return VResult<std::string>(fileContentStream.str());
}

Result Shader::HandleIncludes(CompileInfo* compileInfo)
{
  // We repeatedly search for include statements until there are none left.
  std::string& source = compileInfo->mSource;
  Ds::Vector<SourceChunk>& chunks = compileInfo->mChunks;
  std::regex expr("#include \"([^\"]*)\"");
  std::smatch match;
  while (std::regex_search(source.cbegin(), source.cend(), match, expr)) {
    int includeLine = GetLineNumber(match[0].first - source.begin(), source);
    int currentChunkIndex = compileInfo->GetChunkIndex(includeLine);
    SourceChunk& currentChunk = chunks[currentChunkIndex];
    std::string includeFile = match[1].str();

    // Ignore the inclusion if the file has already been included.
    bool includedGuarded = false;
    for (const SourceChunk& chunk : chunks) {
      if (includeFile == chunk.mFile) {
        source.replace(match[0].first, match[0].second, "");
        includedGuarded = true;
        break;
      }
    }
    if (includedGuarded) {
      continue;
    }

    // Get the included file's content. The include string is used as a path
    // relative to the file it appeared in and relative to the executable.
    size_t pathEnd = currentChunk.mFile.find_last_of('/');
    std::string currentChunkPath(currentChunk.mFile.substr(0, pathEnd + 1));
    includeFile = currentChunkPath + includeFile;
    VResult<std::string> result = GetFileContent(includeFile);
    if (!result.Success()) {
      includeFile = match[1].str();
      result = GetFileContent(includeFile);
      if (!result.Success()) {
        int errorLine = currentChunk.mExcludedLines +
          (includeLine - currentChunk.mStartLine) + 1;
        std::stringstream error;
        error << currentChunk.mFile << "(" << errorLine << ") : "
              << "Failed to include \"" << match[1].str() << "\".\n"
              << result.mError;
        return Result(error.str());
      }
    }
    const std::string& includeContent = result.mValue;
    int includeLineCount = GetLineNumber(includeContent.size(), includeContent);

    // Two new chunks are added for any inclusion. The first chunk is for the
    // code from the included file.
    SourceChunk includeChunk;
    includeChunk.mFile = includeFile;
    includeChunk.mStartLine = includeLine;
    includeChunk.mEndLine = includeLine + includeLineCount;
    includeChunk.mExcludedLines = 0;
    chunks.Insert(currentChunkIndex + 1, includeChunk);

    // The other chunk is for the code that comes after the inclusion.
    int addedNewlineCount = includeLineCount - 1;
    SourceChunk splitChunk;
    splitChunk.mFile = currentChunk.mFile;
    splitChunk.mStartLine = includeChunk.mEndLine;
    splitChunk.mEndLine = currentChunk.mEndLine + addedNewlineCount;
    splitChunk.mExcludedLines = currentChunk.mExcludedLines;
    splitChunk.mExcludedLines += includeLine + 1 - currentChunk.mStartLine;
    chunks.Insert(currentChunkIndex + 2, splitChunk);

    // Modify existing chunks to account for the new inclusion.
    chunks[currentChunkIndex].mEndLine = includeLine;
    for (int i = currentChunkIndex + 3; i < chunks.Size(); ++i) {
      chunks[i].mStartLine += addedNewlineCount;
      chunks[i].mEndLine += addedNewlineCount;
    }

    source.replace(
      match[0].first,
      match[0].second,
      includeContent.begin(),
      includeContent.end());
  }
  return Result();
}

VResult<Ds::Vector<Shader::CompileInfo>> Shader::CollectCompileInfo(
  const std::string& file)
{
  // Get the file content.
  VResult<std::string> fileConentResult = GetFileContent(file);
  if (!fileConentResult.Success()) {
    return Result(fileConentResult.mError);
  }
  const std::string& content = fileConentResult.mValue;

  // Used to track the headers for each subshader in the file.
  struct HeaderData
  {
    // The range of the header [mStart, mEnd).
    size_t mStart;
    size_t mEnd;
    SubType mSubType;
  };
  Ds::Vector<HeaderData> allHeaderData;

  // Define the regular expression for finding the type headers.
  std::string regexString = "#type (";
  regexString += smSubTypeStrings[0];
  for (int i = 1; i < (int)SubType::Count; ++i) {
    regexString += "|";
    regexString += smSubTypeStrings[i];
  }
  regexString += "|[^\n]*)";

  // Find all of the unique type headers.
  std::regex expression(regexString);
  std::smatch match;
  std::string::const_iterator searchStart = content.cbegin();
  while (std::regex_search(searchStart, content.cend(), match, expression)) {
    HeaderData newHeaderData;
    newHeaderData.mStart = match[0].first - content.cbegin();
    newHeaderData.mEnd = match[0].second - content.cbegin();

    // Find the header type and ensure that it's valid.
    newHeaderData.mSubType = GetSubType(match[1].str());
    if (newHeaderData.mSubType == SubType::Invalid) {
      std::stringstream error;
      error << file << "(" << GetLineNumber(newHeaderData.mStart, content)
            << ") : Invalid SubType \"" << match[1].str() << "\".";
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
    size_t sourceEnd;
    if (i == allHeaderData.Size() - 1) {
      sourceEnd = content.cend() - content.cbegin();
    }
    else {
      const HeaderData& nextHeaderData = allHeaderData[i + 1];
      sourceEnd = nextHeaderData.mStart;
    }
    size_t sourceSize = sourceEnd - sourceBegin;

    CompileInfo newCompileInfo;
    newCompileInfo.mSource = content.substr(sourceBegin, sourceSize);
    newCompileInfo.mSubType = headerData.mSubType;

    SourceChunk firstChunk;
    firstChunk.mFile = file;
    firstChunk.mStartLine = 1;
    firstChunk.mEndLine = GetLineNumber(sourceSize, newCompileInfo.mSource) + 1;
    firstChunk.mExcludedLines = GetLineNumber(headerData.mStart, content) - 1;
    newCompileInfo.mChunks.Push(std::move(firstChunk));

    Result result = HandleIncludes(&newCompileInfo);
    if (!result.Success()) {
      return Result(result.mError);
    }
    collection.Push(std::move(newCompileInfo));
  }
  return VResult<Ds::Vector<CompileInfo>>(std::move(collection));
}

} // namespace Gfx