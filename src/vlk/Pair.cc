#include "vlk/Pair.h"
#include "vlk/Parser.h"

namespace Vlk {

// Pair::Value /////////////////////////////////////////////////////////////////
Pair::Value::Value(): mType(Type::Invalid) {}

Pair::Value::Value(Value::Type type): mType(Type::Invalid)
{
  // We initialize mType as Invalid because Init expects mType to be Invalid.
  Init(type);
}

Pair::Value::Value(Value&& other): mType(other.mType)
{
  switch (other.mType)
  {
  case Type::ValueSingle:
    new (&mValueSingle) std::string(Util::Move(other.mValueSingle));
    break;
  case Type::ValueArray:
    new (&mValueArray) Ds::Vector<Value>(Util::Move(other.mValueArray));
    break;
  case Type::PairArray:
    new (&mPairArray) Ds::Vector<Pair>(Util::Move(other.mPairArray));
    break;
  }
}

Pair::Value::~Value()
{
  switch (mType)
  {
  case Type::ValueSingle: mValueSingle.~basic_string(); break;
  case Type::ValueArray: mValueArray.~Vector(); break;
  case Type::PairArray: mPairArray.~Vector(); break;
  }
}

void Pair::Value::Init(Type type)
{
  LogAbortIf(
    mType != Type::Invalid, "This Value has already been initialized.");
  mType = type;
  switch (mType)
  {
  case Type::ValueSingle: new (&mValueSingle) std::string(); break;
  case Type::ValueArray: new (&mValueArray) Ds::Vector<Value>(); break;
  case Type::PairArray: new (&mPairArray) Ds::Vector<Pair>(); break;
  }
}

void Pair::Value::AddDimension(int size, bool leaf)
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

void Pair::Value::ExpectType(Type type)
{
  if (mType == Type::Invalid)
  {
    Init(type);
    return;
  }
  HardExpectType(type);
}

void Pair::Value::HardExpectType(Type type) const
{
  if (mType != type)
  {
    std::stringstream error;
    error << "Function expects a " << type << " Type.";
    LogAbort(error.str().c_str());
  }
}

void Pair::Value::Display(
  std::ostream& os, std::string& indent, const std::string& key) const
{
  // Invalid values will always become empty pair arrays.
  switch (mType)
  {
  case Type::Invalid: os << "{}"; break;
  case Type::ValueSingle: os << mValueSingle; break;
  case Type::ValueArray: DisplayValueArray(os, indent, key); break;
  case Type::PairArray: DisplayPairArray(os, indent); break;
  }
}

void Pair::Value::DisplayValueArray(
  std::ostream& os, std::string& indent, const std::string& key) const
{
  if (BelowPackThreshold())
  {
    if (mValueArray.Empty())
    {
      os << "[]";
      return;
    }
    os << "[";
    mValueArray[0].Display(os, indent, key);
    for (int i = 1; i < mValueArray.Size(); ++i)
    {
      os << ", ";
      mValueArray[i].Display(os, indent, key);
    }
    os << "]";
    return;
  }
  os << "[";
  indent += "  ";
  for (const Value& value : mValueArray)
  {
    os << "\n" << indent;
    value.Display(os, indent, key);
    os << ",";
  }
  indent.erase(indent.size() - 2);
  os << "\n" << indent << "]";
}

bool Pair::Value::BelowPackThreshold() const
{
  int elementCount = 0;
  return !ReachedThreshold(elementCount);
}

bool Pair::Value::ReachedThreshold(int& elementCount) const
{
  HardExpectType(Type::ValueArray);
  if (mValueArray.Empty())
  {
    return false;
  }
  const int countThreshold = 5;
  if (mValueArray[0].mType == Type::ValueSingle)
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

void Pair::Value::DisplayPairArray(std::ostream& os, std::string& indent) const
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

int Pair::Value::Size() const
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

int Pair::Value::AsInt() const
{
  HardExpectType(Type::ValueSingle);
  return std::stoi(mValueSingle);
}

float Pair::Value::AsFloat() const
{
  HardExpectType(Type::ValueSingle);
  return std::stof(mValueSingle);
}

std::string Pair::Value::AsString() const
{
  HardExpectType(Type::ValueSingle);
  return mValueSingle.substr(1, mValueSingle.size() - 2);
}

void Pair::Value::operator=(const char* value)
{
  ExpectType(Type::ValueSingle);
  std::stringstream ss;
  ss << "\"" << value << "\"";
  mValueSingle = ss.str();
}

Pair& Pair::Value::operator()(const char* key)
{
  ExpectType(Value::Type::PairArray);
  LogAbortIf(key[0] == '\0', "Key cannot be an empty string.");
  mPairArray.Emplace(key);
  return mPairArray.Top();
}

Pair& Pair::Value::operator()(const std::string& key)
{
  return (*this)(key.c_str());
}

const Pair& Pair::Value::operator()(const char* key) const
{
  return (*this)(std::string(key));
}

const Pair& Pair::Value::operator()(const std::string& key) const
{
  HardExpectType(Type::PairArray);
  const Pair* foundPair = nullptr;
  for (const Pair& pair : mPairArray)
  {
    if (key.compare(pair.mKey) == 0)
    {
      foundPair = &pair;
      break;
    }
  }
  if (foundPair == nullptr)
  {
    std::stringstream error;
    error << "PairArray did not contain :" << key << ":";
    LogAbort(error.str().c_str());
  }
  return *foundPair;
}

const Pair& Pair::Value::operator()(int pairIndex) const
{
  HardExpectType(Type::PairArray);
  return mPairArray[pairIndex];
}

Pair::Value& Pair::Value::operator[](int index)
{
  ExpectType(Value::Type::ValueArray);
  return mValueArray[index];
}

Pair::Value& Pair::Value::operator[](std::initializer_list<int> sizes)
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

const Pair::Value& Pair::Value::operator[](int valueIndex) const
{
  HardExpectType(Value::Type::ValueArray);
  return mValueArray[valueIndex];
}

std::ostream& operator<<(std::ostream& os, Pair::Value::Type valueType)
{
  switch (valueType)
  {
  case Pair::Value::Type::Invalid: os << "Invalid"; break;
  case Pair::Value::Type::ValueSingle: os << "ValueSingle"; break;
  case Pair::Value::Type::ValueArray: os << "ValueArray"; break;
  case Pair::Value::Type::PairArray: os << "PairArray"; break;
  }
  return os;
}

// Pair ////////////////////////////////////////////////////////////////////////
Pair::Pair() {}

Pair::Pair(const char* key): mKey(key) {}

Pair::Pair(const std::string& key): mKey(key) {}

Pair::Pair(Pair&& other):
  mKey(Util::Move(other.mKey)), mValue(Util::Move(other.mValue))
{}

Pair::Pair(const char* keyText, int length): mKey(keyText, length) {}

int Pair::Size() const
{
  return mValue.Size();
}

Pair& Pair::operator()(const char* key)
{
  return mValue(key);
}
Pair& Pair::operator()(const std::string& key)
{
  return mValue(key);
}
const Pair& Pair::operator()(const char* key) const
{
  return mValue(key);
}
const Pair& Pair::operator()(const std::string& key) const
{
  return mValue(key);
}
const Pair& Pair::operator()(int pairIndex) const
{
  return mValue(pairIndex);
}
Pair::Value& Pair::operator[](int valueIndex)
{
  return mValue[valueIndex];
}
Pair::Value& Pair::operator[](std::initializer_list<int> sizes)
{
  return mValue[sizes];
}
const Pair::Value& Pair::operator[](int valueIndex) const
{
  return mValue[valueIndex];
}
Pair::Value& Pair::operator*()
{
  return mValue;
}
Pair::Value* Pair::operator->()
{
  return &mValue;
}
const Pair::Value& Pair::operator*() const
{
  return mValue;
}
const Pair::Value* Pair::operator->() const
{
  return &mValue;
}

std::ostream& operator<<(std::ostream& os, const Pair& pair)
{
  static std::string indent = "";
  if (!pair.mKey.empty())
  {
    os << ':' << pair.mKey << ": ";
  }
  pair.mValue.Display(os, indent, pair.mKey);
  return os;
}

Util::Result operator>>(std::istream& is, Pair& pair)
{
  pair->HardExpectType(Pair::Value::Type::Invalid);
  std::stringstream content;
  content << is.rdbuf();
  return content.str().c_str() >> pair;
}

Util::Result operator>>(const char* text, Pair& pair)
{
  Parser parser;
  return Util::Result(parser.Parse(text, &pair));
}

} // namespace Vlk
