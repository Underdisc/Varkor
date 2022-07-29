#ifndef util_Memory_h
#define util_Memory_h

#include <utility>

namespace Util {

template<typename T>
void CopyConstructRange(const T* from, T* to, size_t amount)
{
  for (size_t i = 0; i < amount; ++i) {
    new (to + i) T(from[i]);
  }
}

template<typename T>
void CopyAssignRange(const T* from, T* to, size_t amount)
{
  for (size_t i = 0; i < amount; ++i) {
    to[i] = from[i];
  }
}

template<typename T>
void MoveConstructRange(T* from, T* to, size_t amount)
{
  for (size_t i = 0; i < amount; ++i) {
    new (to + i) T(std::move(from[i]));
  }
}

template<typename T>
void MoveAssignRange(T* from, T* to, size_t amount)
{
  for (size_t i = 0; i < amount; ++i) {
    to[i] = std::move(from[i]);
  }
}

template<typename T>
void DestructRange(T* at, size_t amount)
{
  for (size_t i = 0; i < amount; ++i) {
    at[i].~T();
  }
}

template<typename T>
void Fill(T* data, const T& value, size_t amount)
{
  for (size_t i = 0; i < amount; ++i) {
    data[i] = value;
  }
}

} // namespace Util

#endif
