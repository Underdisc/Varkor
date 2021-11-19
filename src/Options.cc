#include <iostream>
#include <mingw/getopt.h>

#include "Error.h"
#include "Options.h"

namespace Options {

std::string nLoadSpace = "";

void ShowHelp()
{
  std::cout
    << "--help | -h: Prints this.\n"
       "--load-editor-space | -s [space.vlk]: The editor will instantly load "
       "and select the space saved within the given file.\n";
}

Util::Result Init(int argc, char* argv[])
{
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
    case 'h': ShowHelp(); return Util::Result("Help requested.");
    case 's': nLoadSpace = optarg; break;
    case '?': return Util::Result("Invalid command line arguments.");
    }
  }
  return Util::Result();
}

} // namespace Options
