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
};

template<typename T>
T Explorer::As() const
{
  return mValue->As<T>();
}

template<typename T>
T Explorer::As(const T& defaultValue) const
{
  if (!Valid()) {
    return defaultValue;
  }
  return mValue->As<T>(defaultValue);
}

} // namespace Vlk

#endif
