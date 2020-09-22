#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

#include "error.h"

namespace Error {

void LogString(const char* string);
std::string FormatFileName(const char* file);

std::ofstream nErrorFile;
bool nUseCout = true;

void Init(const char* logFile)
{
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
  ss << "Error|" << fileName << "|" << line << "> " << reason << std::endl;
  LogString(ss.str().c_str());
}

void Log(const char* reason)
{
  std::stringstream ss;
  ss << "Error> " << reason << std::endl;
  LogString(ss.str().c_str());
}

void Abort(const char* file, int line, const char* reason)
{
  std::stringstream ss;
  std::string fileName = FormatFileName(file);
  ss << "Abort|" << fileName << "|" << line << "> " << reason << std::endl;
  LogString(ss.str().c_str());
  abort();
}

void Abort(const char* reason)
{
  std::stringstream ss;
  ss << "Abort> " << reason << std::endl;
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
    std::cout << string;
  }
  if (nErrorFile.is_open())
  {
    nErrorFile << string;
  }
}

std::string FormatFileName(const char* file)
{
  std::string fileName(file);
  size_t index = fileName.find("src");
  fileName = fileName.substr(index);
  for (int i = 0; i < fileName.size(); ++i)
  {
    if (fileName[i] == '\\')
    {
      fileName[i] = '/';
    }
  }
  return fileName;
}

} // namespace Error
