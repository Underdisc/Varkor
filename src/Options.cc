#include <iostream>
#include <mingw/getopt.h>

#include "Options.h"

namespace Options {

std::string nLoadSpace = "";
std::string nProjectDirectory = "";

void ShowHelp()
{
  std::cout
    << "--help | -h: Prints this.\n"
       "--load-editor-space | -s [space.vlk]: The editor will instantly load "
       "and select the space saved within the given file.\n";
}

Result Init(int argc, char* argv[], const char* projectDirectory)
{
  nProjectDirectory = projectDirectory;
  if (!nProjectDirectory.empty() && nProjectDirectory.back() != '/')
  {
    nProjectDirectory += '/';
  }

  const char* getoptString = "hs:";
  option allOptions[] = {
    {"help", no_argument, NULL, 'h'},
    {"load-editor-space", required_argument, NULL, 's'},
    {0, 0, 0, 0}};
  int currentOp = 0;
  while (currentOp != -1)
  {
    currentOp = getopt_long(argc, argv, getoptString, allOptions, NULL);
    switch (currentOp)
    {
    case 'h': ShowHelp(); return Result("Help requested.");
    case 's': nLoadSpace = optarg; break;
    case '?': return Result("Invalid command line arguments.");
    }
  }
  return Result();
}

std::string PrependResDirectory(const std::string& path)
{
  return nProjectDirectory + "res/" + path;
}

} // namespace Options
