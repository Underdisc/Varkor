#ifndef error_h
#define error_h

#include <fstream>
#include <string>

namespace Error {

void Init(const char* logFile, bool useCout = true);
void Purge();
void Log(const char* file, int line, const char* reason);
void Log(const char* reason);
void Abort(const char* file, int line, const char* reason);
void Abort(const char* reason);
void Abort();

} // namespace Error

#define LogError(reason) Error::Log(__FILE__, __LINE__, reason)
#define LogErrorIf(cond, reason) \
  if (cond)                      \
  {                              \
    LogError(reason);            \
  }

#define LogAbort(reason) Error::Abort(__FILE__, __LINE__, reason)
#define LogAbortIf(cond, reason) \
  if (cond)                      \
  {                              \
    LogAbort(reason);            \
  }

#endif