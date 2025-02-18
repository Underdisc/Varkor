#include <sstream>
#include <string>

#include "Error.h"
#include "Viewport.h"
#include "debug/MemLeak.h"
#include "gfx/Shader.h"
#include "test/Test.h"

template<typename... Args>
void PrintShaderInitResults(const Ds::Vector<std::string>& files) {
  Gfx::Shader shader;
  Result result = shader.Init(files);
  std::cout << "Success: " << result.Success() << '\n';
  if (!result.Success()) {
    std::cout << result.mError << '\n';
  }
}

void FailedIncludeBasic() {
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("FailedIncludeBasic.glf");
  PrintShaderInitResults(files);
}

void FailedInclude() {
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("FailedInclude.glf");
  PrintShaderInitResults(files);
}

void FailedIncludeSubdirectory() {
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("FailedIncludeSubdirectory.glf");
  PrintShaderInitResults(files);
}

void IncludeGuard() {
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("IncludeGuard.glf");
  PrintShaderInitResults(files);
}

void CompilerErrors() {
  Ds::Vector<std::string> files;
  files.Push("Clean.glv");
  files.Push("CompilerErrors.glf");
  PrintShaderInitResults(files);
}

void SingleSource() {
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
}

void UndefFunc() {
  Ds::Vector<std::string> files;
  files.Push("UndefFunc.glv");
  files.Push("Clean.glf");
  PrintShaderInitResults(files);
}

int main() {
  Error::Init();
  Viewport::Init("gfx_Shader", false);
  RunTest(FailedIncludeBasic);
  RunTest(FailedInclude);
  RunTest(FailedIncludeSubdirectory);
  RunTest(IncludeGuard);
  RunTest(CompilerErrors);
  RunTest(SingleSource);
  RunTest(UndefFunc);
  Viewport::Purge();
}
