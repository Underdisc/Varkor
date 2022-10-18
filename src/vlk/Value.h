#ifndef vlk_Value_h
#define vlk_Value_h

#include <initializer_list>
#include <ostream>
#include <sstream>

#include "Result.h"
#include "ds/Vector.h"

namespace Vlk {

struct Pair;
struct Parser;
template<typename T>
struct Serializer;

// Everything in Valkor is a Value. Pairs are a special type of Value that have
// a key string in addition to the Value. This distinction exists because not
// all Values require keys. These keyless Values are contained in ValueArrays.

// We have Pair inherit from Value instead of contain a Value because we don't
// want to define Pair functions that just call the equivalent Value functions.

struct Value
{
public:
  Value();
  Value(Value&& other);
  Value(const Value& other);
  ~Value();
  void Clear();

  Value& operator=(Value&& other);
  Value& operator=(const Value& other);

  bool operator==(const Value& other) const;
  bool operator!=(const Value& other) const;

  Result Read(const char* filename);
  Result Write(const char* filename);
  Result Parse(const char* text);

  enum class Type
  {
    Invalid,
    TrueValue,
    ValueArray,
    PairArray,
  };
  Type GetType() const;
  size_t Size() const;

  constexpr static int smInvalidPairIndex = -1;
  int TryGetPairIndex(const std::string& key) const;
  const Pair* TryGetConstPair(const std::string& key) const;
  const Pair* TryGetConstPair(size_t index) const;
  Pair* TryGetPair(const std::string& key);
  Pair* TryGetPair(size_t index);
  const Value* TryGetConstValue(size_t index) const;
  Value* TryGetValue(size_t index);

  Value& operator()(const std::string& key);
  Value& operator()(size_t index);

  Value& operator[](std::initializer_list<size_t> sizes);
  Value& operator[](size_t index);
  const Value& operator[](size_t index) const;
  template<typename T>
  void PushValue(const T& Value);
  void PopValue();
  void RemoveValue(size_t index);

  template<typename T>
  T As() const;
  template<typename T>
  void operator=(const T& value);

private:
  Type mType;
  union
  {
    std::string mTrueValue;
    Ds::Vector<Value> mValueArray;
    Ds::Vector<Pair> mPairArray;
  };

  Value(Value::Type type);

  void Init(Type type);
  void ExpectType(Type type);
  void HardExpectType(Type type) const;
  void AddDimension(size_t size, bool leaf);
  bool BelowPackThreshold() const;
  bool ReachedThreshold(size_t& elementCount) const;

  friend std::ostream& operator<<(std::ostream& os, Type valueType);
  friend std::ostream& operator<<(std::ostream& os, const Value& value);
  void PrintValue(std::ostream& os, std::string& indent) const;
  void PrintTrueValue(std::ostream& os) const;
  void PrintValueArray(std::ostream& os, std::string& indent) const;
  void PrintPairArray(std::ostream& os, std::string& indent) const;

  friend Pair;
  friend Parser;
  template<typename>
  friend struct Serializer;
  friend Ds::Vector<Value>;
};

template<typename T>
void Value::PushValue(const T& value)
{
  ExpectType(Type::ValueArray);
  mValueArray.Emplace();
  mValueArray.Top() = value;
}

template<>
std::string Value::As<std::string>() const;
template<typename T>
T Value::As() const
{
  HardExpectType(Type::TrueValue);
  std::stringstream ss(mTrueValue);
  T value;
  ss >> value;
  return value;
}

template<typename T>
struct Serializer
{
  static void Serialize(Value& val, const T& value)
  {
    val.ExpectType(Value::Type::TrueValue);
    std::stringstream ss;
    ss << value;
    val.mTrueValue = ss.str();
  }
};

template<typename T>
void Value::operator=(const T& value)
{
  Serializer<T>::Serialize(*this, value);
}

struct Pair: public Value
{
  const std::string& Key() const;
  bool operator==(const Pair& other) const;
  bool operator!=(const Pair& other) const;

private:
  std::string mKey;

  Pair();
  Pair(const std::string& key);

  void PrintPair(std::ostream& os, std::string& indent) const;

  friend Value;
  friend Parser;
  friend Ds::Vector<Pair>;
};

} // namespace Vlk

#endif
