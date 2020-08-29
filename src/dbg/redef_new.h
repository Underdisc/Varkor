#ifndef dbg_redef_new_h
#define dbg_redef_new_h

#ifdef WIN32
  #ifdef _DEBUG
    #define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
  #endif
#endif

#endif
