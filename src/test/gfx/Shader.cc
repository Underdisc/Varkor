#include <sstream>
#include <string>

#include "Error.h"
#include "Viewport.h"
#include "debug/MemLeak.h"
#include "gfx/Shader.h"

void PrintShaderInitResults(const Gfx::Shader::InitInfo& info)
{
  Gfx::Shader shader;
  Result result = shader.Init(info);
  std::cout << "Success: " << result.Success() << std::endl;
  if (!result.Success()) {
    std::cout << result.mError << std::endl;
  }
}

void PrintShaderInitResults(const std::string& file)
{
  Gfx::Shader::InitInfo info;
  info.mScheme = Gfx::Shader::InitInfo::Scheme::Single;
  info.mFile = file;
  PrintShaderInitResults(info);
}

void PrintShaderInitResults(
  const std::string& vertex, const std::string& fragment)
{
  Gfx::Shader::InitInfo info;
  info.mScheme = Gfx::Shader::InitInfo::Scheme::Split;
  info.mVertexFile = vertex;
  info.mFragmentFile = fragment;
  PrintShaderInitResults(info);
}

void FailedIncludeBasic()
{
  std::cout << "<= FailedIncludeBasic =>" << std::endl;
  PrintShaderInitResults("test.vs", "FailedIncludeBasic.fs");
  std::cout << std::endl;
}

void FailedInclude()
{
  std::cout << "<= FailedInclude =>" << std::endl;
  PrintShaderInitResults("test.vs", "FailedInclude.fs");
  std::cout << std::endl;
}

void FailedIncludeSub()
{
  std::cout << "<= FailedIncludeSubdirectory =>" << std::endl;
  PrintShaderInitResults("test.vs", "FailedIncludeSubdirectory.fs");
  std::cout << std::endl;
}

void IncludeGuard()
{
  std::cout << "<= IncludeGuard =>\n";
  PrintShaderInitResults("test.vs", "IncludeGuard.fs");
  std::cout << std::endl;
}

void CompilerErrors()
{
  std::cout << "<= CompilerErrors =>" << std::endl;
  PrintShaderInitResults("test.vs", "CompilerErrors.fs");
  std::cout << std::endl;
}

void SingleSource()
{
  std::cout << "<= SingleSource =>\n";
  const char* prefix = "SingleSource";
  for (int i = 0; i <= 2; ++i) {
    std::stringstream ss;
    ss << prefix << i << ".glsl";
    std::string filename = ss.str();
    std::cout << "-" << filename << "-\n";
    PrintShaderInitResults(filename);
  }
}

int main()
{
  EnableLeakOutput();
  Viewport::Init("gfx_Shader", false);
  FailedIncludeBasic();
  FailedInclude();
  FailedIncludeSub();
  IncludeGuard();
  CompilerErrors();
  SingleSource();
  Viewport::Purge();
}
