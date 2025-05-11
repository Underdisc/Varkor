#include "ds/Hash.h"

namespace Ds {

template<>
size_t Hash(const std::string& str) {
  // This is the Rabin fingerprint hash function (polynomial rolling hash
  // function). The constants were shamelessly taken from wikipedia.
  const size_t base = 256;
  const size_t modulus = 101;
  if (str.empty()) {
    return 0;
  }
  size_t hash = str.back();
  for (int c = (int)str.size() - 2; c >= 0; --c) {
    hash = (hash * base + str[c]) % modulus;
  }
  return hash;
}

} // namespace Ds
