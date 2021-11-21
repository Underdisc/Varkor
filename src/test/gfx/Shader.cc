#include <iostream>

#include "Error.h"
#include "Viewport.h"
#include "debug/MemLeak.h"
#include "gfx/Shader.h"

void PrintShaderInitResults(const char* vertex, const char* fragment)
{
  Gfx::Shader shader;
  Result result = shader.Init(vertex, fragment);
  std::cout << "Success: " << result.Success() << std::endl;
  std::cout << result.mError << std::endl;
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

void CompilerErrors()
{
  std::cout << "<= CompilerErrors =>" << std::endl;
  PrintShaderInitResults("test.vs", "CompilerErrors.fs");
}

int main()
{
  EnableLeakOutput();
  Viewport::Init("gfx_Shader", false);
  FailedIncludeBasic();
  FailedInclude();
  FailedIncludeSub();
  CompilerErrors();
  Viewport::Purge();
}
