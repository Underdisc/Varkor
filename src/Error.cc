#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

#ifdef WIN32
  #include <StackWalker.h>
#else
  #include <backward.hpp>
#endif

#include "Error.h"
#include "debug/MemLeak.h"

namespace Error {

void LogString(const char* string);
std::string FormatFileName(const char* file);
void SignalHandler(int signal);

std::mutex nLogMutex;
std::string nLog;
std::ofstream nFile;
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

  if (logFile == nullptr) {
    return;
  }
  nFile.open(logFile);
  if (!nFile.is_open()) {
    std::cout << "Error log file " << logFile << " failed to open."
              << std::endl;
  }
}

void Purge()
{
  if (nFile.is_open()) {
    nFile.close();
  }
}

void Log(const char* reason)
{
  std::stringstream ss;
  ss << "Error: " << reason;
  LogString(ss.str().c_str());
}

void Log(const char* function, const char* reason)
{
  std::stringstream ss;
  ss << "Error|" << function << ": " << reason;
  LogString(ss.str().c_str());
}

void Log(const char* file, int line, const char* function, const char* reason)
{
  std::stringstream ss;
  std::string filename = FormatFileName(file);
  ss << "Error|" << filename << "(" << line << ")|" << function << ": "
     << reason;
  LogString(ss.str().c_str());
}

void AbortInternal(const char* string)
{
  LogString(string);
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

#ifdef WIN32
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
#endif

void StackTrace()
{
#ifdef WIN32
  LogString("/////////////////////");
  CustomStackWalker stackWalker;
  stackWalker.ShowCallstack();
  LogString("/////////////////////");
#else
  backward::StackTrace trace;
  trace.load_here(32);
  backward::Printer printer;
  std::stringstream traceOutput;
  printer.print(trace, traceOutput);
  LogString(traceOutput.str().c_str());
#endif
}

void LogString(const char* string)
{
  std::stringstream logStream;
  logStream << string << '\n';
  nLogMutex.lock();
  nLog += logStream.str();
  if (nFile.is_open()) {
    nFile << logStream.str();
  }
  if (nUseCout) {
    std::cout << logStream.str();
  }
  nLogMutex.unlock();
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
  case SIGABRT: LogString("SIGABRT: Abort"); break;
  case SIGFPE: LogString("SIGFPE: Floating-Point Exception"); break;
  case SIGILL: LogString("SIGILL: Illegal Instruction"); break;
  case SIGINT: LogString("SIGINT: Interrupt"); break;
  case SIGSEGV: LogString("SIGSEGV: Segfault"); break;
  case SIGTERM: LogString("SIGTERM: Terminate"); break;
  default: LogString("Unknown Signal"); break;
  }
  StackTrace();
  DisableLeakOutput();
  exit(signal);
}

} // namespace Error
