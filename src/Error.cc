#include <csignal>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

#include "Error.h"

namespace Error {

void LogString(const char* string);
std::string FormatFileName(const char* file);
void SignalHandler(int signal);

std::ofstream nErrorFile;
bool nUseCout = true;

void Init(const char* logFile)
{
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

void Log(const char* file, int line, const char* reason)
{
  std::stringstream ss;
  std::string fileName = FormatFileName(file);
  ss << "Error|" << fileName << "|" << line << "> " << reason;
  LogString(ss.str().c_str());
}

void Log(const char* reason)
{
  std::stringstream ss;
  ss << "Error> " << reason;
  LogString(ss.str().c_str());
}

void Abort(const char* file, int line, const char* reason)
{
  std::stringstream ss;
  std::string fileName = FormatFileName(file);
  ss << "Abort|" << fileName << "|" << line << "> " << reason;
  LogString(ss.str().c_str());
  abort();
}

void Abort(const char* reason)
{
  std::stringstream ss;
  ss << "Abort> " << reason;
  LogString(ss.str().c_str());
  abort();
}

void Abort()
{
  std::string str("Abort");
  LogString(str.c_str());
  abort();
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
  std::string fileName(file);
  size_t index = fileName.find("src");
  fileName = fileName.substr(index);
  for (unsigned int i = 0; i < fileName.size(); ++i)
  {
    if (fileName[i] == '\\')
    {
      fileName[i] = '/';
    }
  }
  return fileName;
}

void SignalHandler(int signal)
{
  switch (signal)
  {
  case SIGABRT: LogString("SIGABRT: Abort\n"); break;
  case SIGFPE: LogString("SIGFPE: Floating-Point Exception\n"); break;
  case SIGILL: LogString("SIGILL: Illegal Instruction\n"); break;
  case SIGINT: LogString("SIGINT: Interrupt\n"); break;
  case SIGSEGV: LogString("SIGSEGV: Segfault\n"); break;
  case SIGTERM: LogString("SIGTERM: Terminate\n"); break;
  default: LogString("Unknown Signal\n"); break;
  }
  exit(signal);
}

} // namespace Error
