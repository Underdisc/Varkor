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
  Value(const Value& other);
  Value(Value&& other);
  ~Value();
  void Clear();

  Value& operator=(const Value& other);
  Value& operator=(Value&& other);
  Value& operator=(const Pair& pair);
  Value& operator=(Pair&& pair);

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
  Value& EnsureType(Type type);
  Type GetType() const;
  size_t Size() const;

  constexpr static int smInvalidPairIndex = -1;
  int TryGetPairIndex(const std::string& key) const;
  const Pair* TryGetConstPair(const std::string& key) const;
  const Pair* TryGetConstPair(size_t index) const;
  Pair* TryGetPair(const std::string& key);
  Pair* TryGetPair(size_t index);
  Pair& GetPair(const std::string& key);
  const Value* TryGetConstValue(size_t index) const;
  Value* TryGetValue(size_t index);

  Pair& operator()(const std::string& key);
  Pair& operator()(size_t index);
  bool TryRemovePair(const std::string& key);
  void RemovePair(const std::string& key);

  Value& operator[](std::initializer_list<size_t> sizes);
  Value& operator[](size_t index);
  const Value& operator[](size_t index) const;
  template<typename T>
  void PushValue(const T& value);
  void PushValue();
  void EmplaceValue(Value&& value);
  void PopValue();
  void RemoveValue(size_t index);

  template<typename T>
  T As() const;
  template<typename T>
  T As(const T& defaultValue) const;
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
  void ExpectType(Type type) const;
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
  friend struct Converter;
  friend Ds::Vector<Value>;
};

template<typename T>
struct Converter
{
  static void Serialize(Value& val, const T& value)
  {
    val.EnsureType(Value::Type::TrueValue);
    std::stringstream ss;
    ss << value;
    val.mTrueValue = ss.str();
  }

  static bool Deserialize(const Value& val, T* value)
  {
    if (val.mType != Value::Type::TrueValue) {
      return false;
    }
    std::stringstream ss(val.mTrueValue);
    ss >> *value;
    return true;
  }
};

template<>
struct Converter<std::string>
{
  static void Serialize(Value& val, const std::string& value);
  static bool Deserialize(const Value& val, std::string* value);
};

template<typename T>
void Value::PushValue(const T& value)
{
  PushValue();
  mValueArray.Top() = value;
}

template<typename T>
T Value::As() const
{
  T value;
  bool deserialized = Converter<T>::Deserialize(*this, &value);
  LogAbortIf(!deserialized, "As without a default failed deserialization.");
  return value;
}

template<typename T>
T Value::As(const T& defaultValue) const
{
  T value;
  bool deserialzied = Converter<T>::Deserialize(*this, &value);
  if (!deserialzied) {
    return defaultValue;
  }
  return value;
}

template<typename T>
void Value::operator=(const T& value)
{
  Converter<T>::Serialize(*this, value);
}

struct Pair: public Value
{
  const std::string& Key() const;
  bool operator==(const Pair& other) const;
  bool operator!=(const Pair& other) const;

  template<typename T>
  void operator=(const T& value);

private:
  std::string mKey;

  Pair();
  Pair(const std::string& key);

  void PrintPair(std::ostream& os, std::string& indent) const;

  friend Value;
  friend Parser;
  friend Ds::Vector<Pair>;
};

template<typename T>
void Pair::operator=(const T& value)
{
  (*(Value*)this) = value;
}

} // namespace Vlk

#endif
