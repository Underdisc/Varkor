#include <csignal>
#include <sstream>
#include <stdlib.h>
#include <string>

#include "Error.h"
#include "Log.h"
#include "debug/MemLeak.h"

namespace Error {

std::string FormatFileName(const char* file);
void SignalHandler(int signal);

void Init()
{
  EnableLeakOutput();
  signal(SIGABRT, SignalHandler);
  signal(SIGFPE, SignalHandler);
  signal(SIGILL, SignalHandler);
  signal(SIGINT, SignalHandler);
  signal(SIGSEGV, SignalHandler);
  signal(SIGTERM, SignalHandler);
}

void Log(const char* reason)
{
  std::stringstream ss;
  ss << "Error: " << reason;
  Log::String(ss.str().c_str());
}

void Log(const char* function, const char* reason)
{
  std::stringstream ss;
  ss << "Error>" << function << ": " << reason;
  Log::String(ss.str().c_str());
}

void Log(const char* file, int line, const char* function, const char* reason)
{
  std::stringstream ss;
  std::string filename = FormatFileName(file);
  ss << "Error>" << filename << "(" << line << ")>" << function << ": "
     << reason;
  Log::String(ss.str().c_str());
}

void AbortInternal(const char* string)
{
  Log::String(string);
  abort();
}

void Abort(const char* file, int line, const char* function, const char* reason)
{
  std::stringstream ss;
  std::string filename = FormatFileName(file);
  ss << "Abort>" << filename << "(" << line << ")>" << function << ": "
     << reason;
  AbortInternal(ss.str().c_str());
}

void Abort(const char* reason)
{
  std::stringstream ss;
  ss << "Abort: " << reason;
  AbortInternal(ss.str().c_str());
}

void Abort()
{
  AbortInternal("Abort");
}

std::string FormatFileName(const char* file)
{
  std::string filename(file);
  size_t index = filename.find("src");
  filename = filename.substr(index);
  for (unsigned int i = 0; i < filename.size(); ++i) {
    if (filename[i] == '\\') {
      filename[i] = '/';
    }
  }
  return filename;
}

void SignalHandler(int signal)
{
  switch (signal) {
  case SIGABRT: Log::String("\nSIGABRT: Abort"); break;
  case SIGFPE: Log::String("SIGFPE: Floating-Point Exception"); break;
  case SIGILL: Log::String("SIGILL: Illegal Instruction"); break;
  case SIGINT: Log::String("SIGINT: Interrupt"); break;
  case SIGSEGV: Log::String("SIGSEGV: Segfault"); break;
  case SIGTERM: Log::String("SIGTERM: Terminate"); break;
  default: Log::String("Unknown Signal"); break;
  }
  Log::StackTrace();
  DisableLeakOutput();
  exit(signal);
}

} // namespace Error
