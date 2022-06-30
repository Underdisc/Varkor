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
  PrintShaderInitResults("res/Clean.glv", "res/FailedIncludeBasic.glf");
  std::cout << std::endl;
}

void FailedInclude()
{
  std::cout << "<= FailedInclude =>" << std::endl;
  PrintShaderInitResults("res/Clean.glv", "res/FailedInclude.glf");
  std::cout << std::endl;
}

void FailedIncludeSub()
{
  std::cout << "<= FailedIncludeSubdirectory =>" << std::endl;
  PrintShaderInitResults("res/Clean.glv", "res/FailedIncludeSubdirectory.glf");
  std::cout << std::endl;
}

void IncludeGuard()
{
  std::cout << "<= IncludeGuard =>\n";
  PrintShaderInitResults("res/Clean.glv", "res/IncludeGuard.glf");
  std::cout << std::endl;
}

void CompilerErrors()
{
  std::cout << "<= CompilerErrors =>" << std::endl;
  PrintShaderInitResults("res/Clean.glv", "res/CompilerErrors.glf");
  std::cout << std::endl;
}

void SingleSource()
{
  std::cout << "<= SingleSource =>\n";
  auto testSingleSource = [](const std::string& file)
  {
    std::cout << "=" << file << "=\n";
    PrintShaderInitResults(file);
  };
  testSingleSource("res/SingleSource0.glvif");
  testSingleSource("res/SingleSource1.glvfv");
  testSingleSource("res/SingleSource2.glvf");
  std::cout << std::endl;
}

void UndefFunc()
{
  std::cout << "<= UndefFunc =>\n";
  PrintShaderInitResults("res/UndefFunc.glv", "res/Clean.glf");
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
