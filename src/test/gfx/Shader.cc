#include <sstream>
#include <string>

#include "Error.h"
#include "Viewport.h"
#include "debug/MemLeak.h"
#include "gfx/Shader.h"

template<typename... Args>
void PrintShaderInitResults(const Ds::Vector<std::string>& files)
{
  Gfx::Shader shader;
  Result result = shader.Init(files);
  std::cout << "Success: " << result.Success() << std::endl;
  if (!result.Success()) {
    std::cout << result.mError << std::endl;
  }
}

void FailedIncludeBasic()
{
  std::cout << "<= FailedIncludeBasic =>" << std::endl;
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("FailedIncludeBasic.glf");
  PrintShaderInitResults(files);
  std::cout << std::endl;
}

void FailedInclude()
{
  std::cout << "<= FailedInclude =>" << std::endl;
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("FailedInclude.glf");
  PrintShaderInitResults(files);
  std::cout << std::endl;
}

void FailedIncludeSub()
{
  std::cout << "<= FailedIncludeSubdirectory =>" << std::endl;
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("FailedIncludeSubdirectory.glf");
  PrintShaderInitResults(files);
  std::cout << std::endl;
}

void IncludeGuard()
{
  std::cout << "<= IncludeGuard =>\n";
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("IncludeGuard.glf");
  PrintShaderInitResults(files);
  std::cout << std::endl;
}

void CompilerErrors()
{
  std::cout << "<= CompilerErrors =>" << std::endl;
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("CompilerErrors.glf");
  PrintShaderInitResults(files);
  std::cout << std::endl;
}

void SingleSource()
{
  std::cout << "<= SingleSource =>\n";
  auto testSingleSource = [](const std::string& file)
  {
    std::cout << "=" << file << "=\n";
    Ds::Vector<std::string> files;
    files.Push(file);
    PrintShaderInitResults(files);
  };
  testSingleSource("SingleSource0.glvif");
  testSingleSource("SingleSource1.glvfv");
  testSingleSource("SingleSource2.glvf");
  std::cout << std::endl;
}

void UndefFunc()
{
  std::cout << "<= UndefFunc =>\n";
  Ds::Vector<std::string> files;
  files.Push("UndefFunc.glv");
  files.Push("Clean.glf");
  PrintShaderInitResults(files);
}

int main()
{
  EnableLeakOutput();
  Error::Init();
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
