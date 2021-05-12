#ifndef Error_h
#define Error_h

namespace Error {

void Init(const char* logFile = nullptr);
void Purge();
void Log(const char* file, int line, const char* reason);
void Log(const char* reason);
void LogGlStatus();
void Abort(const char* file, int line, const char* reason);
void Abort(const char* reason);
void Abort();
void StackTrace();

extern bool nUseCout;

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