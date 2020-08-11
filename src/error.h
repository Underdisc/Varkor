#ifndef Error_h
#define Error_h

#include <fstream>
#include <string>

class Error
{
public:
  static void Init(const char* log_file, bool use_cout = true);
  static void Purge();
  static void Log(const char* file, int line, const char* reason);
  static void Log(const char* reason);
  static void Abort(const char* file, int line, const char* reason);
  static void Abort(const char* reason);
  static void Abort();

private:
  static void LogString(const char* string);
  static std::string FormatFileName(const char* file);
  static std::ofstream _error_file;
  static bool _use_cout;
  Error() {}
};

#define LogError(reason) Error::Log(__FILE__, __LINE__, reason)
#define LogErrorIf(cond, reason) \
  if (cond) LogError(reason)

#define LogAbort(reason) Error::Abort(__FILE__, __LINE__, reason)
#define LogAbortIf(cond, reason) \
  if (cond) LogAbort(reason)

#endif