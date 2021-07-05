#ifndef vlk_Pair_h
#define vlk_Pair_h

#include <initializer_list>
#include <istream>
#include <ostream>
#include <sstream>

#include "ds/Vector.h"
#include "util/Utility.h"

namespace Vlk {

struct Parser;

struct Pair
{
  // There is a distinction between Pairs and Values because Values can be
  // elements within ValueArrays. If there were no distinction, every element
  // in a ValueArray would contain an unnecessary key string.
  struct Value
  {
  public:
    int Size() const;
    int AsInt() const;
    float AsFloat() const;
    std::string AsString() const;

    template<typename T>
    void operator=(const T& value);
    void operator=(const char* value);

    Pair& operator()(const char* key);
    Pair& operator()(const std::string& key);
    const Pair& operator()(const char* key) const;
    const Pair& operator()(const std::string& key) const;
    const Pair& operator()(int pairIndex) const;
    Value& operator[](int index);
    Value& operator[](std::initializer_list<int> sizes);
    const Value& operator[](int index) const;

  private:
    enum class Type
    {
      Invalid,
      ValueSingle,
      ValueArray,
      PairArray,
    };

    Type mType;
    union
    {
      std::string mValueSingle;
      Ds::Vector<Value> mValueArray;
      Ds::Vector<Pair> mPairArray;
    };

    Value();
    Value(Value::Type type);
    Value(Value&& other);
    ~Value();

    void Init(Type type);
    void AddDimension(int size, bool leaf);
    void ExpectType(Type type);
    void HardExpectType(Type type) const;
    void Display(
      std::ostream& os, std::string& indent, const std::string& key) const;
    void DisplayValueArray(
      std::ostream& os, std::string& indent, const std::string& key) const;
    bool BelowPackThreshold() const;
    bool ReachedThreshold(int& elementCount) const;
    void DisplayPairArray(std::ostream& os, std::string& indent) const;

    friend Ds::Vector<Value>;
    friend Pair;
    friend Parser;
    friend std::ostream& operator<<(std::ostream& os, const Pair& pair);
    friend std::ostream& operator<<(std::ostream& os, Type valueType);
    friend Util::Result operator>>(std::istream& is, Pair& pair);
  };

public:
  std::string mKey;

  Pair();
  Pair(const char* key);
  Pair(const std::string& key);
  Pair(Pair&& other);

  int Size() const;

  template<typename t>
  void operator=(const t& value);

  Pair& operator()(const char* key);
  Pair& operator()(const std::string& key);
  const Pair& operator()(const char* key) const;
  const Pair& operator()(const std::string& key) const;
  const Pair& operator()(int pairIndex) const;
  Value& operator[](int valueIndex);
  Value& operator[](std::initializer_list<int> sizes);
  const Value& operator[](int valueIndex) const;
  Value& operator*();
  Value* operator->();
  const Value& operator*() const;
  const Value* operator->() const;

private:
  Value mValue;

  Pair(const char* keyText, int length);

  friend Ds::Vector<Pair>;
  friend Parser;
  friend std::ostream& operator<<(std::ostream& os, const Pair& pair);
  friend Util::Result operator>>(std::istream& is, Pair& pair);
  friend Util::Result operator>>(const char* text, Pair& pair);
};

template<typename T>
void Pair::Value::operator=(const T& value)
{
  ExpectType(Type::ValueSingle);
  std::stringstream ss;
  ss << value;
  mValueSingle = ss.str();
}

template<typename t>
void Pair::operator=(const t& value)
{
  mValue = value;
}

typedef Pair::Value Value;

} // namespace Vlk

#endif
