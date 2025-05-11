#ifndef ds_Hash_h
#define ds_Hash_h

#include <string>

namespace Ds {
template<typename T>
size_t Hash(const T& key) {
  return (size_t)key;
}
} // namespace Ds

template<>
size_t Ds::Hash(const std::string& str);

#endif
