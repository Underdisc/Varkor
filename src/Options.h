#ifndef Options_h
#define Options_h

#include <string>

#include "Result.h"

namespace Options {

extern std::string nLoadSpace;

Result Init(int argc, char* argv[]);

} // namespace Options

#endif
