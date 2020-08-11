#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "error.h"

std::ofstream Error::_error_file = std::ofstream();
bool Error::_use_cout = true;

void Error::Init(const char* log_file, bool use_cout)
{
  _error_file.open(log_file);
  if (!_error_file.is_open())
  {
    std::cout << "Error log file " << log_file << " failed to open."
              << std::endl;
  }
  _use_cout = use_cout;
}

void Error::Purge()
{
  if (_error_file.is_open())
  {
    _error_file.close();
  }
}

void Error::Log(const char* file, int line, const char* reason)
{
  std::stringstream ss;
  std::string file_name = FormatFileName(file);
  ss << "Error|" << file_name << "|" << line << "> " << reason << std::endl;
  LogString(ss.str().c_str());
}

void Error::Log(const char* reason)
{
  std::stringstream ss;
  ss << "Error> " << reason << std::endl;
  LogString(ss.str().c_str());
}

void Error::Abort(const char* file, int line, const char* reason)
{
  std::stringstream ss;
  std::string file_name = FormatFileName(file);
  ss << "Abort|" << file_name << "|" << line << "> " << reason << std::endl;
  LogString(ss.str().c_str());
  abort();
}

void Error::Abort(const char* reason)
{
  std::stringstream ss;
  ss << "Abort> " << reason << std::endl;
  LogString(ss.str().c_str());
  abort();
}

void Error::Abort()
{
  std::string str("Abort");
  LogString(str.c_str());
  abort();
}

void Error::LogString(const char* string)
{
  if (_use_cout)
  {
    std::cout << string;
  }
  if (_error_file.is_open())
  {
    _error_file << string;
  }
}

std::string Error::FormatFileName(const char* file)
{
  std::string file_name(file);
  size_t index = file_name.find("src");
  file_name = file_name.substr(index);
  for (int i = 0; i < file_name.size(); ++i)
  {
    if (file_name[i] == '\\')
    {
      file_name[i] = '/';
    }
  }
  return file_name;
}
