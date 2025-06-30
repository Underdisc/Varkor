#ifndef Error_h
#define Error_h

#include <mutex>
#include <string>

namespace Error {

void Init();
void Log(const char* reason);
void Log(const char* function, const char* reason);
void Log(const char* file, int line, const char* function, const char* reason);
void Abort(
  const char* file, int line, const char* function, const char* reason);
void Abort(const char* reason);
void Abort();

} // namespace Error

#define LogError(reason) Error::Log(__FILE__, __LINE__, __FUNCTION__, reason)
#define LogErrorIf(cond, reason) \
  if (cond) {                    \
    LogError(reason);            \
  }

#define LogAbort(reason) Error::Abort(__FILE__, __LINE__, __FUNCTION__, reason)
#define LogAbortIf(cond, reason) \
  if (cond) {                    \
    LogAbort(reason);            \
  }

#define Assert(cond) LogAbortIf(!(cond), "Assert failed")

#endif
