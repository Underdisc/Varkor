#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

#include <StackWalker.h>
#include <glad/glad.h>

#include "Error.h"
#include "debug/MemLeak.h"

namespace Error {

void LogString(const char* string);
std::string FormatFileName(const char* file);
void SignalHandler(int signal);

std::ofstream nErrorFile;
bool nUseCout = true;

void Init(const char* logFile)
{
  EnableLeakOutput();

  signal(SIGABRT, SignalHandler);
  signal(SIGFPE, SignalHandler);
  signal(SIGILL, SignalHandler);
  signal(SIGINT, SignalHandler);
  signal(SIGSEGV, SignalHandler);
  signal(SIGTERM, SignalHandler);

  if (logFile == nullptr)
  {
    return;
  }
  nErrorFile.open(logFile);
  if (!nErrorFile.is_open())
  {
    std::cout << "Error log file " << logFile << " failed to open."
              << std::endl;
  }
}

void Purge()
{
  if (nErrorFile.is_open())
  {
    nErrorFile.close();
  }
}

void Log(const char* file, int line, const char* function, const char* reason)
{
  std::stringstream ss;
  std::string filename = FormatFileName(file);
  ss << "Error|" << filename << "(" << line << ")|" << function << ": "
     << reason;
  LogString(ss.str().c_str());
}

void Log(const char* reason)
{
  std::stringstream ss;
  ss << "Error: " << reason;
  LogString(ss.str().c_str());
}

void LogGlStatus()
{
  GLenum error = glGetError();
  std::stringstream ss;
  // clang-format off
  switch (error)
  {
  case GL_NO_ERROR:
    ss << "GL_NO_ERROR"; break;
  case GL_INVALID_ENUM:
    ss << "GL_INVALID_ENUM"; break;
  case GL_INVALID_VALUE:
    ss << "GL_INVALID_VALUE"; break;
  case GL_INVALID_OPERATION:
    ss << "GL_INVALID_OPERATION"; break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    ss << "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
  case GL_OUT_OF_MEMORY:
    ss << "GL_OUT_OF_MEMORY"; break;
  case GL_STACK_UNDERFLOW:
    ss << "GL_STACK_UNDERFLOW"; break;
  case GL_STACK_OVERFLOW:
    ss << "GL_STACK_OVERFLOW"; break;
  }
  // clang-format on
  LogString(ss.str().c_str());
}

void AbortInternal(const char* string)
{
  LogString(string);
  DisableLeakOutput();
  abort();
}

void Abort(const char* file, int line, const char* function, const char* reason)
{
  std::stringstream ss;
  std::string filename = FormatFileName(file);
  ss << "Abort|" << filename << "(" << line << ")|" << function << ": "
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
  std::string str("Abort");
  AbortInternal(str.c_str());
}

class CustomStackWalker: public StackWalker
{
public:
  CustomStackWalker(): StackWalker() {}

private:
  virtual void OnCallstackEntry(CallstackEntryType type, CallstackEntry& entry)
  {
    std::stringstream ss;
    ss << entry.undFullName << " => " << entry.lineFileName << "("
       << entry.lineNumber << ")";
    LogString(ss.str().c_str());
  }
};

void StackTrace()
{
  LogString("/////////////////////");
  CustomStackWalker stackWalker;
  stackWalker.ShowCallstack();
  LogString("/////////////////////");
}

void LogString(const char* string)
{
  if (nUseCout)
  {
    std::cout << string << std::endl;
  }
  if (nErrorFile.is_open())
  {
    nErrorFile << string << std::endl;
  }
}

std::string FormatFileName(const char* file)
{
  std::string filename(file);
  size_t index = filename.find("src");
  filename = filename.substr(index);
  for (unsigned int i = 0; i < filename.size(); ++i)
  {
    if (filename[i] == '\\')
    {
      filename[i] = '/';
    }
  }
  return filename;
}

void SignalHandler(int signal)
{
  switch (signal)
  {
  case SIGABRT: LogString("SIGABRT: Abort"); break;
  case SIGFPE: LogString("SIGFPE: Floating-Point Exception"); break;
  case SIGILL: LogString("SIGILL: Illegal Instruction"); break;
  case SIGINT: LogString("SIGINT: Interrupt"); break;
  case SIGSEGV: LogString("SIGSEGV: Segfault"); break;
  case SIGTERM: LogString("SIGTERM: Terminate"); break;
  default: LogString("Unknown Signal"); break;
  }
  StackTrace();
  exit(signal);
}

} // namespace Error
