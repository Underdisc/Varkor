#include <iostream>
#include <getopt.h>

#include "Options.h"

namespace Options {

Ds::Vector<std::string> nLoadLayers;
std::string nProjectDirectory = "";

void ShowHelp()
{
  std::cout << "--help | -h: Prints this.\n"
               "--load-layer | -l [layer.vlk]: The editor will instantly load "
               "and select the layer saved within the given file.\n";
}

Result Init(int argc, char* argv[], const char* projectDirectory)
{
  nProjectDirectory = projectDirectory;
  if (!nProjectDirectory.empty() && nProjectDirectory.back() != '/') {
    nProjectDirectory += '/';
  }

  const char* getoptString = "hl:";
  option allOptions[] = {
    {"help", no_argument, NULL, 'h'},
    {"load-layer", required_argument, NULL, 'l'},
    {0, 0, 0, 0}};
  int currentOp = 0;
  while (currentOp != -1) {
    currentOp = getopt_long(argc, argv, getoptString, allOptions, NULL);
    switch (currentOp) {
    case 'h': ShowHelp(); return Result("Help requested.");
    case 'l': nLoadLayers.Push(optarg); break;
    case '?': return Result("Invalid command line arguments.");
    }
  }
  return Result();
}

} // namespace Options
