#ifndef vlk_Pair_h
#define vlk_Pair_h

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
  template<typename T>
  T As() const;
  size_t Size() const;

  const Pair* TryGetPair(const std::string& key) const;
  const Pair* TryGetPair(size_t index) const;
  const Value* TryGetValue(size_t index) const;

  Pair& operator()(const char* key);
  Pair& operator()(const std::string& key);
  const Pair& operator()(size_t index) const;

  Value& operator[](std::initializer_list<size_t> sizes);
  Value& operator[](size_t index);
  const Value& operator[](size_t index) const;

  template<typename T>
  void operator=(const T& value);
  void operator=(const char* value);
  void operator=(const std::string& value);

protected:
  enum class Type
  {
    Invalid,
    String,
    ValueArray,
    PairArray,
  };

  Type mType;
  union
  {
    std::string mString;
    Ds::Vector<Value> mValueArray;
    Ds::Vector<Pair> mPairArray;
  };

  Value();
  Value(Value::Type type);
  Value(Value&& other);
  ~Value();

  void Init(Type type);
  void ExpectType(Type type);
  void HardExpectType(Type type) const;
  void AddDimension(size_t size, bool leaf);
  bool BelowPackThreshold() const;
  bool ReachedThreshold(size_t& elementCount) const;
  void PrintValue(std::ostream& os, std::string& indent) const;
  void PrintValueArray(std::ostream& os, std::string& indent) const;
  void PrintPairArray(std::ostream& os, std::string& indent) const;

  friend Ds::Vector<Value>;
  friend Parser;
  friend std::ostream& operator<<(std::ostream& os, Type valueType);
  friend std::ostream& operator<<(std::ostream& os, const Pair& pair);
};

template<typename T>
T Value::As() const
{
  HardExpectType(Type::String);
  std::stringstream ss(mString);
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
  ExpectType(Type::String);
  std::stringstream ss;
  ss << value;
  mString = ss.str();
}

struct Pair: public Value
{
  Pair();
  Pair(const char* key);
  Pair(const std::string& key);

  Util::Result Read(const char* filename);
  Util::Result Write(const char* filename);
  Util::Result Parse(const char* text);

  const std::string& Key() const;

  template<typename T>
  void operator=(const T& value);

private:
  std::string mKey;

  friend Parser;
  friend std::ostream& operator<<(std::ostream& os, const Pair& pair);
};

template<typename T>
void Pair::operator=(const T& value)
{
  Value& base = *this;
  base = value;
}

} // namespace Vlk

#endif
