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
std::string GetShortTypename()
{
  std::string fullName(typeid(T).name());
  std::string::size_type start = fullName.find_last_of(" :");
  if (start == std::string::npos)
  {
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

struct Result
{
  std::string mError;
  Result(): mError("") {}
  Result(Result&& other): mError(Util::Move(other.mError)) {}
  Result(std::string&& error): mError(Util::Move(error)) {}
  Result(std::string& error): mError(error) {}
  Result(const char* error): mError(error) {}
  Result(bool success)
  {
    if (success)
    {
      mError = "";
    } else
    {
      mError = "Failed";
    }
  }
  bool Success()
  {
    return mError.size() == 0;
  }
  Result& operator=(Result&& other)
  {
    mError = Util::Move(other.mError);
    return *this;
  }
};

} // namespace Util

#endif
