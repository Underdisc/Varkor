#include "vlk/Extensions.h"

namespace Vlk {

template<>
void Value::operator=(const Vec3& value)
{
  operator[]({3});
  for (int i = 0; i < 3; ++i) {
    operator[](i) = value[i];
  }
}

template<>
Vec3 Explorer::AsInternal<Vec3>(const Vec3& defaultValue) const
{
  Vec3 value;
  for (int i = 0; i < 3; ++i) {
    Explorer element = operator[](i);
    if (!element.Valid()) {
      return defaultValue;
    }
    value[i] = element.As<float>();
  }
  return value;
}

} // namespace Vlk