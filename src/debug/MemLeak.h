// This is for basic memory debugging under windows while building with msvc
// debug. Calling InitMemLeakOutput at any point during execution will print all
// memory leaks to stdout after the program exits. This file should be included
// with any file that calls new. alloc should be used a replacement for the new
// keyword in any case where new memory is allocted. It should not be used when
// new is used for placement new. The alloc keyword will allow the memory leak
// output to contain file and line number information for memory blocks.

#ifndef debug_MemLeak_h
#define debug_MemLeak_h

// todo: Investigate new ways to track memory leaks that don't involve replacing
// the new keyword for lines that perform actual memory allocation.
#if defined WIN32 && defined _DEBUG
  #define _CRTDBG_MAP_ALLOC
  #include <crtdbg.h>
  #define alloc new (_CLIENT_BLOCK, __FILE__, __LINE__)
#else
  #define alloc new
#endif

void EnableLeakOutput();
void DisableLeakOutput();

#endif
