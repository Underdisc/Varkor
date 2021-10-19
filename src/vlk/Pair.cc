#include <fstream>

#include "Error.h"
#include "vlk/Pair.h"
#include "vlk/Parser.h"

namespace Vlk {

// Value ///////////////////////////////////////////////////////////////////////
Value::Value(): mType(Type::Invalid) {}

Value::Value(Value::Type type): mType(Type::Invalid)
{
  // We initialize mType in the initializer list with Invalid because Init
  // expects mType to be Invalid.
  Init(type);
}

Value::Value(Value&& other): mType(other.mType)
{
  switch (other.mType)
  {
  case Type::String:
    new (&mString) std::string(Util::Move(other.mString));
    break;
  case Type::ValueArray:
    new (&mValueArray) Ds::Vector<Value>(Util::Move(other.mValueArray));
    break;
  case Type::PairArray:
    new (&mPairArray) Ds::Vector<Pair>(Util::Move(other.mPairArray));
    break;
  }
}

Value::~Value()
{
  switch (mType)
  {
  case Type::String: mString.~basic_string(); break;
  case Type::ValueArray: mValueArray.~Vector(); break;
  case Type::PairArray: mPairArray.~Vector(); break;
  }
}

void Value::Init(Type type)
{
  LogAbortIf(
    mType != Type::Invalid, "This Value has already been initialized.");
  mType = type;
  switch (mType)
  {
  case Type::String: new (&mString) std::string(); break;
  case Type::ValueArray: new (&mValueArray) Ds::Vector<Value>(); break;
  case Type::PairArray: new (&mPairArray) Ds::Vector<Pair>(); break;
  }
}

void Value::ExpectType(Type type)
{
  if (mType == Type::Invalid)
  {
    Init(type);
    return;
  }
  HardExpectType(type);
}

void Value::HardExpectType(Type type) const
{
  if (mType != type)
  {
    std::stringstream error;
    error << "Function expects a " << type << " Type.";
    LogAbort(error.str().c_str());
  }
}

void Value::AddDimension(size_t size, bool leaf)
{
  ExpectType(Type::ValueArray);
  if (mValueArray.Size() == 0)
  {
    if (leaf)
    {
      mValueArray.Resize(size);
    } else
    {
      mValueArray.Resize(size, Value::Type::ValueArray);
    }
    return;
  }
  for (Value& subValue : mValueArray)
  {
    subValue.AddDimension(size, leaf);
  }
}

bool Value::BelowPackThreshold() const
{
  size_t elementCount = 0;
  return !ReachedThreshold(elementCount);
}

bool Value::ReachedThreshold(size_t& elementCount) const
{
  HardExpectType(Type::ValueArray);
  if (mValueArray.Empty())
  {
    return false;
  }
  const size_t countThreshold = 5;
  if (mValueArray[0].mType == Type::String)
  {
    elementCount += mValueArray.Size();
    return elementCount > countThreshold;
  }
  for (const Value& subValue : mValueArray)
  {
    if (subValue.ReachedThreshold(elementCount))
    {
      return true;
    }
  }
  return false;
}

void Value::PrintValue(std::ostream& os, std::string& indent) const
{
  // Invalid values will always become empty pair arrays.
  switch (mType)
  {
  case Type::Invalid: os << "{}"; break;
  case Type::String: os << mString; break;
  case Type::ValueArray: PrintValueArray(os, indent); break;
  case Type::PairArray: PrintPairArray(os, indent); break;
  }
}

void Value::PrintValueArray(std::ostream& os, std::string& indent) const
{
  if (BelowPackThreshold())
  {
    if (mValueArray.Empty())
    {
      os << "[]";
      return;
    }
    os << "[";
    mValueArray[0].PrintValue(os, indent);
    for (size_t i = 1; i < mValueArray.Size(); ++i)
    {
      os << ", ";
      mValueArray[i].PrintValue(os, indent);
    }
    os << "]";
    return;
  }
  os << "[";
  indent += "  ";
  for (const Value& value : mValueArray)
  {
    os << "\n" << indent;
    value.PrintValue(os, indent);
    os << ",";
  }
  indent.erase(indent.size() - 2);
  os << "\n" << indent << "]";
}

void Value::PrintPairArray(std::ostream& os, std::string& indent) const
{
  if (mPairArray.Empty())
  {
    os << "{}";
    return;
  }
  os << "{\n";
  indent += "  ";
  for (const Pair& pair : mPairArray)
  {
    os << indent << pair << "\n";
  }
  indent.erase(indent.size() - 2);
  os << indent << "}";
}

template<>
int Value::As<int>() const
{
  HardExpectType(Type::String);
  return std::stoi(mString);
}

template<>
float Value::As<float>() const
{
  HardExpectType(Type::String);
  return std::stof(mString);
}

template<>
std::string Value::As<std::string>() const
{
  HardExpectType(Type::String);
  return mString.substr(1, mString.size() - 2);
}

size_t Value::Size() const
{
  if (mType != Type::ValueArray && mType != Type::PairArray)
  {
    std::stringstream error;
    error << "Size expects a " << Type::ValueArray << " or " << Type::PairArray
          << "type.";
    LogAbort(error.str().c_str());
  }
  if (mType == Type::ValueArray)
  {
    return mValueArray.Size();
  }
  return mPairArray.Size();
}

const Pair* Value::TryGetPair(const std::string& key) const
{
  HardExpectType(Type::PairArray);
  for (const Pair& pair : mPairArray)
  {
    if (key == pair.Key())
    {
      return &pair;
    }
  }
  return nullptr;
}

const Pair* Value::TryGetPair(size_t index) const
{
  HardExpectType(Type::PairArray);
  if (index < 0 || mPairArray.Size() <= index)
  {
    return nullptr;
  }
  return &mPairArray[index];
}

const Value* Value::TryGetValue(size_t index) const
{
  HardExpectType(Type::ValueArray);
  if (index < 0 || mValueArray.Size() <= index)
  {
    return nullptr;
  }
  return &mValueArray[index];
}

Pair& Value::operator()(const char* key)
{
  ExpectType(Type::PairArray);
  LogAbortIf(key[0] == '\0', "Key cannot be an empty string.");
  mPairArray.Emplace(key);
  return mPairArray.Top();
}

Pair& Value::operator()(const std::string& key)
{
  return (*this)(key.c_str());
}

const Pair& Value::operator()(size_t index) const
{
  HardExpectType(Type::PairArray);
  return mPairArray[index];
}

Value& Value::operator[](std::initializer_list<size_t> sizes)
{
  auto it = sizes.begin();
  auto itE = sizes.end();
  while (it < itE)
  {
    bool leafArray = it == itE - 1;
    AddDimension(*it, leafArray);
    ++it;
  }
  return *this;
}

Value& Value::operator[](size_t index)
{
  HardExpectType(Type::ValueArray);
  return mValueArray[index];
}

const Value& Value::operator[](size_t index) const
{
  HardExpectType(Type::ValueArray);
  return mValueArray[index];
}

void Value::operator=(const char* value)
{
  ExpectType(Type::String);
  std::stringstream ss;
  ss << "\"" << value << "\"";
  mString = ss.str();
}

void Value::operator=(const std::string& value)
{
  *this = value.c_str();
}

std::ostream& operator<<(std::ostream& os, Value::Type valueType)
{
  switch (valueType)
  {
  case Value::Type::Invalid: os << "Invalid"; break;
  case Value::Type::String: os << "String"; break;
  case Value::Type::ValueArray: os << "ValueArray"; break;
  case Value::Type::PairArray: os << "PairArray"; break;
  }
  return os;
}

// Pair ////////////////////////////////////////////////////////////////////////
Pair::Pair() {}
Pair::Pair(const char* key): mKey(key) {}
Pair::Pair(const std::string& key): mKey(key) {}

Util::Result Pair::Read(const char* filename)
{
  // Read the file's content.
  std::ifstream stream(filename, std::ifstream::in);
  if (!stream.is_open())
  {
    std::stringstream error;
    error << filename << " failed to open while reading.";
    return Util::Result(error.str());
  }
  std::stringstream content;
  content << stream.rdbuf();
  stream.close();

  // Parse the content.
  Util::Result result = Parse(content.str().c_str());
  if (!result.Success())
  {
    std::stringstream error;
    error << filename << " failed parsing: " << result.mError;
    return Util::Result(error.str());
  }
  return result;
}

Util::Result Pair::Write(const char* filename)
{
  std::ofstream stream(filename, std::ofstream::out);
  if (!stream.is_open())
  {
    std::stringstream error;
    error << filename << " failed to open while writing.";
    return Util::Result(error.str());
  }
  stream << *this;
  return Util::Result(true);
}

Util::Result Pair::Parse(const char* text)
{
  LogAbortIf(
    mKey != "" || mType != Value::Type::Invalid,
    "Parse can only be used on a completely uninitialized Pair.");
  Parser parser;
  return parser.Parse(text, this);
}

const std::string& Pair::Key() const
{
  return mKey;
}

std::ostream& operator<<(std::ostream& os, const Pair& pair)
{
  if (!pair.mKey.empty())
  {
    os << ':' << pair.mKey << ": ";
  }
  static std::string indent = "";
  pair.PrintValue(os, indent);
  return os;
}

} // namespace Vlk
