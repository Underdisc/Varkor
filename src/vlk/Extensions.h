#ifndef vlk_Extensions_h
#define vlk_Extensions_h

#include "math/Vector.h"
#include "vlk/Valkor.h"

namespace Vlk {

template<typename T, unsigned int N>
struct Serializer<Math::Vector<T, N>>
{
  static void Serialize(Value& val, const Math::Vector<T, N>& value)
  {
    val[{N}];
    for (int i = 0; i < N; ++i) {
      val[i] = value[i];
    }
  }
};

template<typename T, unsigned int N>
struct Deserializer<Math::Vector<T, N>>
{
  static bool Deserialize(const Explorer& ex, Math::Vector<T, N>* value)
  {
    for (int i = 0; i < N; ++i) {
      Explorer element = ex[i];
      if (!element.Valid()) {
        return false;
      }
      (*value)[i] = element.As<T>();
    }
    return true;
  }
};

} // namespace Vlk

#endif
