#ifndef vlk_Value_h
#define vlk_Value_h

#include <initializer_list>
#include <ostream>
#include <sstream>

#include "ds/Vector.h"
#include "util/Utility.h"

namespace Vlk {

struct Pair;
struct Parser;

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
  ~Value();

  Util::Result Read(const char* filename);
  Util::Result Write(const char* filename);
  Util::Result Parse(const char* text);

  template<typename T>
  T As() const;
  size_t Size() const;

  const Pair* TryGetPair(const std::string& key) const;
  const Pair* TryGetPair(size_t index) const;
  const Value* TryGetValue(size_t index) const;

  Value& operator()(const char* key);
  Value& operator()(const std::string& key);
  const Pair& operator()(size_t index) const;

  Value& operator[](std::initializer_list<size_t> sizes);
  Value& operator[](size_t index);
  const Value& operator[](size_t index) const;

  template<typename T>
  void operator=(const T& value);
  void operator=(const char* value);
  void operator=(const std::string& value);

private:
  enum class Type
  {
    Invalid,
    TrueValue,
    ValueArray,
    PairArray,
  };

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
  friend Ds::Vector<Value>;
};

template<typename T>
T Value::As() const
{
  HardExpectType(Type::TrueValue);
  std::stringstream ss(mTrueValue);
  T value;
  ss >> value;
  return value;
}
template<>
int Value::As<int>() const;
template<>
float Value::As<float>() const;
template<>
std::string Value::As<std::string>() const;

template<typename T>
void Value::operator=(const T& value)
{
  ExpectType(Type::TrueValue);
  std::stringstream ss;
  ss << value;
  mTrueValue = ss.str();
}

struct Pair: public Value
{
  const std::string& Key() const;

private:
  std::string mKey;

  Pair();
  Pair(const char* key);
  Pair(const std::string& key);

  void PrintPair(std::ostream& os, std::string& indent) const;

  friend Value;
  friend Parser;
  friend Ds::Vector<Pair>;
};

} // namespace Vlk

#endif
