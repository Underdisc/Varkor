#ifndef Result_h
#define Result_h

#include <utility>

struct Result
{
  std::string mError;
  bool Success()
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

template<typename T>
struct ValueResult: public Result
{
  T mValue;

  ValueResult(const T& value): Result(), mValue(value) {}
  ValueResult(T&& value): Result(), mValue(std::move(value)) {}
  ValueResult(const std::string& error, T&& value):
    Result(error), mValue(std::move(value))
  {}
  ValueResult(Result&& result, const T& value):
    Result(std::move(result)), mValue(value)
  {}
  ValueResult(Result&& result, T&& value):
    Result(std::move(result)), mValue(std::move(value))
  {}
  ValueResult(ValueResult<T>&& other):
    Result(std::move(other.mError)), mValue(std::move(other.mValue))
  {}
  ValueResult<T>& operator=(ValueResult<T>&& other)
  {
    mError = std::move(other.mError);
    mValue = std::move(other.mValue);
    return *this;
  }
};

#endif
