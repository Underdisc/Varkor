#ifndef Options_h
#define Options_h

#include <string>

#include "Result.h"

namespace Options {

extern std::string nLoadSpace;
extern std::string nProjectDirectory;

Result Init(int argc, char* argv[], const char* projectDirectory);

} // namespace Options

#endif
