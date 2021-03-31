#ifndef util_Utility_h
#define util_Utility_h

#include <string>

namespace Util {

template<typename T>
constexpr T&& Move(T& arg)
{
  return static_cast<T&&>(arg);
}

template<typename T>
constexpr T&& Forward(T& arg)
{
  return static_cast<T&&>(arg);
}

template<typename T>
std::string GetFullName()
{
  std::string fullName(typeid(T).name());
  std::string::size_type loc = fullName.find_last_of(" ");
  fullName.erase(0, loc + 1);
  return fullName;
}

template<typename T>
std::string GetLastName()
{
  std::string fullName(typeid(T).name());
  std::string::size_type loc = fullName.find_last_of(" :");
  fullName.erase(0, loc + 1);
  return fullName;
}

} // namespace Util

#endif
