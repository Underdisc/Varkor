#ifndef util_Memory_h
#define util_Memory_h

namespace Util {

template<typename T>
void Copy(T* from, T* to, size_t amount)
{
  for (size_t i = 0; i < amount; ++i)
  {
    to[i] = from[i];
  }
}

template<typename T>
void MoveData(T* from, T* to, size_t amount)
{
  for (size_t i = 0; i < amount; ++i)
  {
    to[i] = Move(from[i]);
  }
}

template<typename T>
void Fill(T* data, const T& value, size_t amount)
{
  for (size_t i = 0; i < amount; ++i)
  {
    data[i] = value;
  }
}

} // namespace Util

#endif
