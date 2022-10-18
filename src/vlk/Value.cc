#include <fstream>
#include <utility>

#include "Error.h"
#include "vlk/Parser.h"
#include "vlk/Value.h"

namespace Vlk {

Value::Value(): mType(Type::Invalid) {}

Value::Value(Value::Type type): mType(Type::Invalid)
{
  // We initialize mType in the initializer list with Invalid because Init
  // expects mType to be Invalid.
  Init(type);
}

Value::Value(const Value& other): mType(Type::Invalid)
{
  *this = other;
}

Value::Value(Value&& other)
{
  *this = std::move(other);
}

Value::~Value()
{
  Clear();
}

void Value::Clear()
{
  switch (mType) {
  case Type::TrueValue: mTrueValue.~basic_string(); break;
  case Type::ValueArray: mValueArray.~Vector(); break;
  case Type::PairArray: mPairArray.~Vector(); break;
  }
  mType = Type::Invalid;
}

Value& Value::operator=(const Value& other)
{
  Clear();
  mType = other.mType;
  // clang-format off
  switch (other.mType) {
  case Type::TrueValue:
    new (&mTrueValue) std::string(other.mTrueValue);
    break;
  case Type::ValueArray:
    new (&mValueArray) Ds::Vector<Value>(other.mValueArray);
    break;
  case Type::PairArray:
    new (&mPairArray) Ds::Vector<Pair>(other.mPairArray);
    break;
  }
  // clang-format on
  return *this;
}

Value& Value::operator=(Value&& other)
{
  Clear();
  mType = other.mType;
  switch (other.mType) {
  case Type::TrueValue:
    new (&mTrueValue) std::string(std::move(other.mTrueValue));
    break;
  case Type::ValueArray:
    new (&mValueArray) Ds::Vector<Value>(std::move(other.mValueArray));
    break;
  case Type::PairArray:
    new (&mPairArray) Ds::Vector<Pair>(std::move(other.mPairArray));
    break;
  }
  other.Clear();
  return *this;
}

bool Value::operator==(const Value& other) const
{
  if (mType != other.mType) {
    return false;
  }
  switch (mType) {
  case Type::TrueValue:
    if (mTrueValue != other.mTrueValue) {
      return false;
    }
    break;
  case Type::ValueArray:
    if (mValueArray.Size() != other.mValueArray.Size()) {
      return false;
    }
    for (int i = 0; i < mValueArray.Size(); ++i) {
      if (mValueArray[i] != other.mValueArray[i]) {
        return false;
      }
    }
    break;
  case Type::PairArray:
    if (mPairArray.Size() != other.mPairArray.Size()) {
      return false;
    }
    for (int i = 0; i < mPairArray.Size(); ++i) {
      if (mPairArray[i] != other.mPairArray[i]) {
        return false;
      }
    }
    break;
  }
  return true;
}

bool Value::operator!=(const Value& other) const
{
  return !(*this == other);
}

void Value::Init(Type type)
{
  LogAbortIf(
    mType != Type::Invalid, "This Value has already been initialized.");
  mType = type;
  switch (mType) {
  case Type::TrueValue: new (&mTrueValue) std::string(); break;
  case Type::ValueArray: new (&mValueArray) Ds::Vector<Value>(); break;
  case Type::PairArray: new (&mPairArray) Ds::Vector<Pair>(); break;
  }
}

void Value::ExpectType(Type type)
{
  if (mType == Type::Invalid) {
    Init(type);
    return;
  }
  HardExpectType(type);
}

Value::Type Value::GetType() const
{
  return mType;
}

void Value::HardExpectType(Type type) const
{
  if (mType != type) {
    std::stringstream error;
    error << "Function expects a " << type << " Type.";
    LogAbort(error.str().c_str());
  }
}

void Value::AddDimension(size_t size, bool leaf)
{
  ExpectType(Type::ValueArray);
  if (mValueArray.Size() == 0) {
    if (leaf) {
      mValueArray.Resize(size);
    }
    else {
      mValueArray.Resize(size, Value::Type::ValueArray);
    }
    return;
  }
  for (Value& subValue : mValueArray) {
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
  const size_t countThreshold = 5;
  switch (mType) {
  case Type::ValueArray:
    for (const Value& subValue : mValueArray) {
      if (subValue.ReachedThreshold(elementCount)) {
        return true;
      }
    }
    return false;
  case Type::PairArray: return true;
  case Type::TrueValue: ++elementCount; return elementCount > countThreshold;
  }
  LogAbort("Function expects an initialized Value.");
  return true;
}

Result Value::Read(const char* filename)
{
  // Read the file's content.
  std::ifstream stream(filename, std::ifstream::in);
  if (!stream.is_open()) {
    std::stringstream error;
    error << filename << " failed to open.";
    return Result(error.str());
  }
  std::stringstream content;
  content << stream.rdbuf();
  stream.close();

  // Parse the content.
  Result result = Parse(content.str().c_str());
  if (!result.Success()) {
    std::stringstream error;
    error << filename << result.mError;
    return Result(error.str());
  }
  return result;
}

Result Value::Write(const char* filename)
{
  std::ofstream stream(filename, std::ofstream::out);
  if (!stream.is_open()) {
    std::stringstream error;
    error << filename << " failed to open while writing.";
    return Result(error.str());
  }
  stream << *this;
  return Result();
}

Result Value::Parse(const char* text)
{
  LogAbortIf(
    mType != Value::Type::Invalid,
    "Parse can only be used on an uninitialized Value.");
  Parser parser;
  return parser.Parse(text, this);
}

size_t Value::Size() const
{
  if (mType != Type::ValueArray && mType != Type::PairArray) {
    std::stringstream error;
    error << "Size expects a " << Type::ValueArray << " or " << Type::PairArray
          << "type.";
    LogAbort(error.str().c_str());
  }
  if (mType == Type::ValueArray) {
    return mValueArray.Size();
  }
  return mPairArray.Size();
}

int Value::TryGetPairIndex(const std::string& key) const
{
  if (mType != Type::PairArray) {
    return smInvalidPairIndex;
  }
  for (size_t i = 0; i < mPairArray.Size(); ++i) {
    if (key == mPairArray[i].mKey) {
      return (int)i;
    }
  }
  return smInvalidPairIndex;
}

const Pair* Value::TryGetConstPair(const std::string& key) const
{
  int pairIndex = TryGetPairIndex(key);
  if (pairIndex == smInvalidPairIndex) {
    return nullptr;
  }
  return &mPairArray[pairIndex];
}

const Pair* Value::TryGetConstPair(size_t index) const
{
  if (mType != Type::PairArray || index < 0 || mPairArray.Size() <= index) {
    return nullptr;
  }
  return &mPairArray[index];
}

Pair* Value::TryGetPair(const std::string& key)
{
  return const_cast<Pair*>(TryGetConstPair(key));
}

Pair* Value::TryGetPair(size_t index)
{
  return const_cast<Pair*>(TryGetConstPair(index));
}

const Value* Value::TryGetConstValue(size_t index) const
{
  if (mType != Type::ValueArray || index < 0 || mValueArray.Size() <= index) {
    return nullptr;
  }
  return &mValueArray[index];
}

Value* Value::TryGetValue(size_t index)
{
  return const_cast<Value*>(TryGetConstValue(index));
}

Value& Value::operator()(const std::string& key)
{
  ExpectType(Type::PairArray);
  LogAbortIf(key.empty(), "Key cannot be an empty string.");
  Pair* pair = TryGetPair(key);
  if (pair == nullptr) {
    mPairArray.Emplace(key);
    return mPairArray.Top();
  }
  return *pair;
}

Value& Value::operator()(size_t index)
{
  HardExpectType(Type::PairArray);
  return mPairArray[index];
}

Value& Value::operator[](std::initializer_list<size_t> sizes)
{
  auto it = sizes.begin();
  auto itE = sizes.end();
  while (it < itE) {
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

void Value::PopValue()
{
  HardExpectType(Type::ValueArray);
  mValueArray.Pop();
}

void Value::RemoveValue(size_t index)
{
  HardExpectType(Type::ValueArray);
  mValueArray.Remove(index);
}

template<>
std::string Value::As<std::string>() const
{
  HardExpectType(Type::TrueValue);
  return mTrueValue;
}

std::ostream& operator<<(std::ostream& os, Value::Type valueType)
{
  switch (valueType) {
  case Value::Type::Invalid: os << "Invalid"; break;
  case Value::Type::TrueValue: os << "TrueValue"; break;
  case Value::Type::ValueArray: os << "ValueArray"; break;
  case Value::Type::PairArray: os << "PairArray"; break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Value& value)
{
  static std::string indent = "";
  value.PrintValue(os, indent);
  return os;
}

void Value::PrintValue(std::ostream& os, std::string& indent) const
{
  // Invalid values will always become empty pair arrays.
  switch (mType) {
  case Type::Invalid: os << "{}"; break;
  case Type::TrueValue: PrintTrueValue(os); break;
  case Type::ValueArray: PrintValueArray(os, indent); break;
  case Type::PairArray: PrintPairArray(os, indent); break;
  }
}

void Value::PrintTrueValue(std::ostream& os) const
{
  std::string trueText;
  trueText.push_back('\'');
  for (size_t i = 0; i < mTrueValue.size(); ++i) {
    switch (mTrueValue[i]) {
    case '\'': trueText.append("\\\'"); break;
    case '\\': trueText.append("\\\\"); break;
    default: trueText.push_back(mTrueValue[i]);
    }
  }
  trueText.push_back('\'');
  os << trueText;
}

void Value::PrintValueArray(std::ostream& os, std::string& indent) const
{
  if (mValueArray.Empty()) {
    os << "[]";
    return;
  }
  if (BelowPackThreshold()) {
    os << "[";
    mValueArray[0].PrintValue(os, indent);
    for (size_t i = 1; i < mValueArray.Size(); ++i) {
      os << ", ";
      mValueArray[i].PrintValue(os, indent);
    }
    os << "]";
    return;
  }
  indent += "  ";
  os << "[\n" << indent;
  mValueArray[0].PrintValue(os, indent);
  for (size_t i = 1; i < mValueArray.Size(); ++i) {
    os << ",\n" << indent;
    mValueArray[i].PrintValue(os, indent);
  }
  indent.erase(indent.size() - 2);
  os << "\n" << indent << "]";
}

void Value::PrintPairArray(std::ostream& os, std::string& indent) const
{
  if (mPairArray.Empty()) {
    os << "{}";
    return;
  }
  os << "{\n";
  indent += "  ";
  for (const Pair& pair : mPairArray) {
    pair.PrintPair(os, indent);
  }
  indent.erase(indent.size() - 2);
  os << indent << "}";
}

Pair::Pair() {}

Pair::Pair(const std::string& key): mKey(key) {}

const std::string& Pair::Key() const
{
  return mKey;
}

bool Pair::operator==(const Pair& other) const
{
  if (mKey != other.mKey) {
    return false;
  }
  return Value::operator==(other);
}

bool Pair::operator!=(const Pair& other) const
{
  return !(*this == other);
}

void Pair::PrintPair(std::ostream& os, std::string& indent) const
{
  os << indent << ':' << mKey << ": ";
  PrintValue(os, indent);
  os << '\n';
}

} // namespace Vlk
