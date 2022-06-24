#ifndef Options_h
#define Options_h

#include <string>

#include "Result.h"
#include "ds/Vector.h"

namespace Options {

extern Ds::Vector<std::string> nLoadSpaces;
extern std::string nProjectDirectory;

Result Init(int argc, char* argv[], const char* projectDirectory);

} // namespace Options

#endif
