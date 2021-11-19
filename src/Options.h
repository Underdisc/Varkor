#ifndef Options_h
#define Options_h

#include <string>

#include "util/Utility.h"

namespace Options {

extern std::string nLoadSpace;

Util::Result Init(int argc, char* argv[]);

} // namespace Options

#endif
