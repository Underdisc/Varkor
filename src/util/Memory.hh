#ifndef util_Memory_hh
#define util_Memory_hh

namespace Util {

template<typename T>
void Copy(T* from, T* to, int amount)
{
  for(int i = 0; i < amount; ++i)
  {
    to[i] = from[i];
  }
}

} // namespace Util

#endif
