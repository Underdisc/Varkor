#include <mutex>
#include <string>

namespace Log {

extern std::mutex nLogMutex;
extern std::string nLog;
extern bool nUseCout;

void Init(const char* logFile = nullptr);
void Purge();

void StackTrace();
void String(const char* string);
void String(const std::string& string);

} // namespace Log