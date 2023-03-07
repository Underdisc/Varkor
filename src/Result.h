#ifndef Result_h
#define Result_h

#include <ostream>
#include <string>
#include <utility>

struct Result
{
  std::string mError;
  bool Success() const
  {
    return mError.size() == 0;
  }

  Result(): mError("") {}
  Result(std::string&& error): mError(std::move(error)) {}
  Result(const std::string& error): mError(error) {}
  Result(const char* error): mError(error) {}
  Result(Result&& other): mError(std::move(other.mError)) {}
  Result& operator=(Result&& other)
  {
    mError = std::move(other.mError);
    return *this;
  }
};

std::ostream& operator<<(std::ostream& os, const Result& rhs);

template<typename T>
struct ValueResult: public Result
{
  T mValue;

  ValueResult(const T& value): mValue(value), Result() {}
  ValueResult(T&& value): mValue(std::move(value)), Result() {}
  ValueResult(const T& value, Result&& result):
    mValue(value), Result(std::move(result))
  {}
  ValueResult(T&& value, Result&& result):
    mValue(std::move(value)), Result(std::move(result))
  {}
  ValueResult(Result&& result): mValue(), Result(std::move(result)) {}
  ValueResult(ValueResult<T>&& other):
    mValue(std::move(other.mValue)), Result(std::move(other.mError))
  {}
  ValueResult<T>& operator=(ValueResult<T>&& other)
  {
    mError = std::move(other.mError);
    mValue = std::move(other.mValue);
    return *this;
  }
};
template<typename T>
using VResult = ValueResult<T>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const VResult<T>& rhs)
{
  if (rhs.Success()) {
    os << rhs.mValue;
  }
  else {
    os << rhs.mError;
  }
  return os;
}

#endif
