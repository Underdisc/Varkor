#ifndef Profiler_h
#define Profiler_h

namespace Profiler {

void Start(const char* name);
void EndStart(const char* name);
void End();
void Show();
void Clear();

} // namespace Profiler

namespace Pf = Profiler;

#endif
