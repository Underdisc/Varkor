// This is for basic memory debugging under windows while building with msvc
// debug. Calling InitMemLeakOutput at any point during execution will print
// all memory leaks to stdout after the program exits. redef_new.h will redefine
// new and should be included in any file that calls new. Every memory leak will
// contain a file and line number describing where the allocation was made when
// new is redefined.

#ifdef WIN32
  // The order of the next three lines must be maintained.
  #define _CRTDBG_MAP_ALLOC
  #include <crtdbg.h>
  #include <stdlib.h>
#endif

void InitMemLeakOutput()
{
#ifdef WIN32
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
  _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}
