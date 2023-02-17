#include <iostream>

#include <backward.hpp>

#include "Log.h"

namespace Log {

std::mutex nLogMutex;
std::string nLog;
std::ofstream nFile;
bool nUseCout = true;

void Init(const char* logFile)
{
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

void StackTrace()
{
  backward::TraceResolver dummyResolver;
  backward::StackTrace trace;
  trace.load_here(32);
  backward::Printer printer;
  printer.snippet = false;
  std::stringstream traceOutput;
  printer.print(trace, traceOutput);
  String(traceOutput.str().c_str());
}

void String(const char* string)
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

void String(const std::string& string)
{
  String(string.c_str());
}

} // namespace Log