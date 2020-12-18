#ifndef util_Utility_h
#define util_Utility_h

namespace Util {

template<typename T>
constexpr T&& Move(T& arg)
{
  return static_cast<T&&>(arg);
}

} // namespace Util

#endif
