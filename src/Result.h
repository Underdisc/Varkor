#ifndef Result_h
#define Result_h

#include "util/Utility.h"

struct Result
{
  std::string mError;
  bool Success()
  {
    return mError.size() == 0;
  }

  Result(): mError("") {}
  Result(std::string&& error): mError(Util::Move(error)) {}
  Result(const std::string& error): mError(error) {}
  Result(const char* error): mError(error) {}
  Result(Result&& other): mError(Util::Move(other.mError)) {}
  Result& operator=(Result&& other)
  {
    mError = Util::Move(other.mError);
    return *this;
  }
};

template<typename T>
struct ValueResult: public Result
{
  T mValue;

  ValueResult(const T& value): Result(), mValue(value) {}
  ValueResult(T&& value): Result(), mValue(Util::Move(value)) {}
  ValueResult(const std::string& error, T&& value):
    Result(error), mValue(Util::Move(value))
  {}
  ValueResult(Result&& result, const T& value):
    Result(Util::Move(result)), mValue(value)
  {}
  ValueResult(Result&& result, T&& value):
    Result(Util::Move(result)), mValue(Util::Move(value))
  {}
  ValueResult(ValueResult<T>&& other):
    Result(Util::Move(other.mError)), mValue(Util::Move(other.mValue))
  {}
  ValueResult<T>& operator=(ValueResult<T>&& other)
  {
    mError = Util::Move(other.mError);
    mValue = Util::Move(other.mValue);
    return *this;
  }
};

#endif
