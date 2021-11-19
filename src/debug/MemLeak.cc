#include "MemLeak.h"

void EnableLeakOutput()
{
#if defined WIN32 && defined _DEBUG
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

void DisableLeakOutput()
{
#if defined WIN32 && defined _DEBUG
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
#endif
}
