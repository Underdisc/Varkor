#ifndef util_Utility_h
#define util_Utility_h

#include <string>
#include <typeinfo>

namespace Util {

template<typename T>
std::string GetFullTypename()
{
  std::string fullName(typeid(T).name());
  return fullName;
}

template<typename T>
std::string GetShortTypename()
{
  std::string fullName(typeid(T).name());
  std::string::size_type start = fullName.find_last_of(" :");
  if (start == std::string::npos) {
    return fullName;
  }
  return fullName.substr(start + 1);
}

template<bool, class T = void>
struct EnableIf
{};

template<class T>
struct EnableIf<true, T>
{
  typedef T Type;
};

} // namespace Util

#endif
