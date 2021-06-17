#include "Error.h"

#include "lang/Writer.h"

namespace Lang {

Writer::Writer():
  mExpectingKey(false),
  mExpectedWrites(0),
  mArrayDepth(0),
  mValueArrayDepth(0),
  mCurrentRequest(Request::None)
{}

void Writer::VerifyCompleteRequest()
{
  LogAbortIf(mCurrentRequest != Request::None, "Previous Request incomplete.");
}

Writer& Writer::operator<<(Request request)
{
  switch (request)
  {
  case Request::Value:
    VerifyCompleteRequest();
    mExpectingKey = true;
    mExpectedWrites = 2;
    mCurrentRequest = request;
    break;

  case Request::ValueArray:
    if (mCurrentRequest == Request::ValueArray)
    {
      SValueArray();
    } else
    {
      VerifyCompleteRequest();
      mExpectingKey = true;
      mExpectedWrites = 1;
      mCurrentRequest = request;
    }
    break;

  case Request::EndValueArray:
    LogAbortIf(mExpectedWrites == 1, "ValueArray expects a Key.");
    LogAbortIf(mCurrentRequest != Request::ValueArray, "ValueArray not open.");
    EValueArray();
    if (mValueArrayDepth == 0)
    {
      mCurrentRequest = Request::None;
    }
    break;

  case Request::Array:
    VerifyCompleteRequest();
    mExpectingKey = true;
    mExpectedWrites = 1;
    mCurrentRequest = request;
    break;

  case Request::EndArray:
    VerifyCompleteRequest();
    LogAbortIf(mArrayDepth == 0, "All Arrays have been closed.");
    EArray();
    break;
  }
  return *this;
}

void Writer::WriteKey(const char* string)
{
  --mExpectedWrites;
  mExpectingKey = false;
  mBuffer << mIndent << string << ": ";
}

void Writer::Write(const char* string)
{
  LogAbortIf(mCurrentRequest == Request::None, "No request made.");

  switch (mCurrentRequest)
  {
  case Request::Value:
    if (mExpectedWrites == 2)
    {
      WriteKey(string);
    } else if (mExpectedWrites == 1)
    {
      --mExpectedWrites;
      mBuffer << string << "\n";
      mCurrentRequest = Request::None;
    }
    break;

  case Request::ValueArray:
    if (mExpectedWrites == 1)
    {
      WriteKey(string);
      SValueArray();
    } else
    {
      mBuffer << mIndent << string << "\n";
    }
    break;

  case Request::Array:
    if (mExpectedWrites == 1)
    {
      WriteKey(string);
      SArray();
      mCurrentRequest = Request::None;
    }
    break;
  }
}

Writer& Writer::operator<<(const char* value)
{
  std::stringstream ss;
  if (mExpectingKey == true)
  {
    ss << value;
  } else
  {
    ss << "\"" << value << "\"";
  }
  Write(ss.str().c_str());
  return *this;
}

const std::stringstream& Writer::GetCompleteBuffer()
{
  VerifyCompleteRequest();
  LogAbortIf(mArrayDepth > 0, "All arrays have not been closed.");
  return mBuffer;
}

void Writer::SValueArray()
{
  if (mValueArrayDepth > 0)
  {
    mBuffer << mIndent;
  }
  ++mValueArrayDepth;
  mBuffer << "[\n";
  mIndent += "  ";
}

void Writer::EValueArray()
{
  --mValueArrayDepth;
  mIndent.erase(mIndent.size() - 2);
  mBuffer << mIndent << "]\n";
}

void Writer::SArray()
{
  ++mArrayDepth;
  mIndent += "  ";
  mBuffer << "{\n";
}

void Writer::EArray()
{
  --mArrayDepth;
  mIndent.erase(mIndent.size() - 2);
  mBuffer << mIndent << "}\n";
}

} // namespace Lang
