#ifndef vlk_Explorer_h
#define vlk_Explorer_h

#include <string>

#include "vlk/Value.h"

namespace Vlk {

template<typename T>
struct Deserializer;

// The Explorer's purpose is to browse through the Value tree and keep track of
// where in the Value tree the Explorer is. Unlike the Value, the Explorer
// provides functions that logs errors any time a value access attempt is made
// and does not succeed. Imagine Value's as files and directories. Explorers are
// similar to the browser used to explore those files and directories.

// Explorers should be used when deserializing data whereas Values should be
// used when serializing data.

struct Explorer
{
  Explorer(const Value& rootVal);

  template<typename T>
  T As() const;
  template<typename T>
  T As(const T& defaultValue) const;

  std::string Path() const;
  const std::string& Key() const;
  size_t Size() const;
  bool Valid() const;
  bool Valid(Value::Type type) const;

  Explorer operator()(const std::string& key) const;
  Explorer operator()(size_t index) const;
  Explorer operator[](size_t index) const;

private:
  Explorer(const Explorer* parent);
  Explorer(const Explorer* parent, const Pair* pair);
  Explorer(const Explorer* parent, const Value* value, size_t index);

  const Explorer* mParent;
  const Value* mValue;
  size_t mIndex;
  bool mIsPair;

  template<typename>
  friend struct Deserializer;
};

template<typename T>
struct Deserializer
{
  static bool Deserialize(const Explorer& ex, T* value)
  {
    *value = ex.mValue->As<T>();
    return true;
  }
};

template<typename T>
T Explorer::As() const
{
  LogAbortIf(!Valid(), "As without a default used on an invalid Explorer.");
  T value;
  bool deserialized = Deserializer<T>::Deserialize(*this, &value);
  LogAbortIf(!deserialized, "As without a default failed deserialization.");
  return value;
}

template<typename T>
T Explorer::As(const T& defaultValue) const
{
  if (!Valid()) {
    return defaultValue;
  }
  T value;
  bool deserialized = Deserializer<T>::Deserialize(*this, &value);
  if (!deserialized) {
    return defaultValue;
  }
  return value;
}

} // namespace Vlk

#endif
