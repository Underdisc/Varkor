// This is for basic memory debugging under windows while building with msvc
// debug. Calling InitMemLeakOutput at any point during execution will print
// all memory leaks to stdout after the program exits. This file should be
// included with any file that calls new because new will be redefined to track
// memory leaks.

#ifndef debug_MemLeak_h
#define debug_MemLeak_h

#if defined WIN32 && defined _DEBUG
  // The order of the next three lines must be maintained.
  #define _CRTDBG_MAP_ALLOC
  #include <crtdbg.h>
  #include <stdlib.h>

  #define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

void InitMemLeakOutput();

#endif
