#ifndef lang_Writer_h
#define lang_Writer_h

#include <sstream>
#include <string>

namespace Vlk {

struct Writer
{
  Writer();

  enum class Request
  {
    None,
    Value,
    ValueArray,
    EndValueArray,
    Array,
    EndArray
  };
  Writer& operator<<(Request request);

  void Write(const char* string);
  Writer& operator<<(const char* value);
  template<typename T>
  Writer& operator<<(const T& value);

  const std::stringstream& GetCompleteBuffer();

private:
  bool mExpectingKey;
  int mExpectedWrites;
  int mArrayDepth;
  int mValueArrayDepth;
  Request mCurrentRequest;
  std::string mIndent;
  std::stringstream mBuffer;

  void WriteKey(const char* string);
  void SValueArray();
  void EValueArray();
  void SArray();
  void EArray();
  void VerifyCompleteRequest();
};

template<typename T>
Writer& Writer::operator<<(const T& value)
{
  std::stringstream string;
  string << value;
  Write(string.str().c_str());
  return *this;
}

} // namespace Vlk

typedef Vlk::Writer::Request Request;

#endif
