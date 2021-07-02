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

template<bool, class T = void>
struct EnableIf
{};

template<class T>
struct EnableIf<true, T>
{
  typedef T Type;
};

struct Result
{
  std::string mError;
  Result(): mError("") {}
  Result(Result&& other): mError(Util::Move(other.mError)) {}
  Result(std::string&& error): mError(Util::Move(error)) {}
  Result(std::string& error): mError(error) {}
  Result(const char* error): mError(error) {}
  bool Success()
  {
    return mError.size() == 0;
  }
};

} // namespace Util

#endif
