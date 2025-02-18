#ifndef util_Memory_h
#define util_Memory_h

#include <cstdlib>
#include <utility>

namespace Util {

template<typename T>
void DefaultConstruct(void* data) {
  new (data) T;
}

template<typename T>
void CopyConstruct(void* from, void* to) {
  new (to) T(*(T*)from);
}

template<typename T>
void MoveConstruct(void* from, void* to) {
  new (to) T(std::move(*(T*)from));
}

template<typename T>
void MoveAssign(void* from, void* to) {
  *(T*)to = std::move(*(T*)from);
}

template<typename T>
void Destruct(void* data) {
  (*(T*)data).~T();
}

template<typename T>
void CopyConstructRange(const T* from, T* to, size_t amount) {
  for (size_t i = 0; i < amount; ++i) {
    new (to + i) T(from[i]);
  }
}

template<typename T>
void CopyAssignRange(const T* from, T* to, size_t amount) {
  for (size_t i = 0; i < amount; ++i) {
    to[i] = from[i];
  }
}

template<typename T>
void MoveConstructRange(T* from, T* to, size_t amount) {
  for (size_t i = 0; i < amount; ++i) {
    new (to + i) T(std::move(from[i]));
  }
}

template<typename T>
void MoveAssignRange(T* from, T* to, size_t amount) {
  for (size_t i = 0; i < amount; ++i) {
    to[i] = std::move(from[i]);
  }
}

template<typename T>
void DestructRange(T* at, size_t amount) {
  for (size_t i = 0; i < amount; ++i) {
    at[i].~T();
  }
}

template<typename T>
void Fill(T* data, const T& value, size_t amount) {
  for (size_t i = 0; i < amount; ++i) {
    data[i] = value;
  }
}

template<typename T>
void Swap(T* data, size_t indexA, size_t indexB) {
  T& a = *(data + indexA);
  T& b = *(data + indexB);
  T temp = std::move(a);
  a = std::move(b);
  b = std::move(temp);
}

} // namespace Util

#endif
