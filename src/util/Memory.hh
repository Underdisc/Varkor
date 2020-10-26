#ifndef util_Memory_hh
#define util_Memory_hh

namespace Util {

template<typename T>
void Copy(T* from, T* to, int amount)
{
  for (int i = 0; i < amount; ++i)
  {
    to[i] = from[i];
  }
}

template<typename T>
void Fill(T* data, const T& value, int amount)
{
  for (int i = 0; i < amount; ++i)
  {
    data[i] = value;
  }
}

} // namespace Util

#endif
