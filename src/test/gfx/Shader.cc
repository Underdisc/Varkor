#include <sstream>
#include <string>

#include "Error.h"
#include "Viewport.h"
#include "debug/MemLeak.h"
#include "gfx/Shader.h"

template<typename... Args>
void PrintShaderInitResults(Args&&... args)
{
  Gfx::Shader::InitInfo info;
  info.Prep(args...);
  Gfx::Shader shader;
  Result result = shader.Init(info);
  std::cout << "Success: " << result.Success() << std::endl;
  if (!result.Success()) {
    std::cout << result.mError << std::endl;
  }
}

void FailedIncludeBasic()
{
  std::cout << "<= FailedIncludeBasic =>" << std::endl;
  PrintShaderInitResults("Clean.vs", "FailedIncludeBasic.fs");
  std::cout << std::endl;
}

void FailedInclude()
{
  std::cout << "<= FailedInclude =>" << std::endl;
  PrintShaderInitResults("Clean.vs", "FailedInclude.fs");
  std::cout << std::endl;
}

void FailedIncludeSub()
{
  std::cout << "<= FailedIncludeSubdirectory =>" << std::endl;
  PrintShaderInitResults("Clean.vs", "FailedIncludeSubdirectory.fs");
  std::cout << std::endl;
}

void IncludeGuard()
{
  std::cout << "<= IncludeGuard =>\n";
  PrintShaderInitResults("Clean.vs", "IncludeGuard.fs");
  std::cout << std::endl;
}

void CompilerErrors()
{
  std::cout << "<= CompilerErrors =>" << std::endl;
  PrintShaderInitResults("Clean.vs", "CompilerErrors.fs");
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
    std::cout << "=" << filename << "=\n";
    PrintShaderInitResults(filename);
  }
  std::cout << std::endl;
}

void UndefFunc()
{
  std::cout << "<= UndefFunc =>\n";
  PrintShaderInitResults("UndefFunc.vs", "Clean.fs");
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
  UndefFunc();
  Viewport::Purge();
}
