#ifndef vlk_Explorer_h
#define vlk_Explorer_h

#include <string>

#include "vlk/Pair.h"

namespace Vlk {

// The Explorer's purpose is to browse through the Value tree and keep track of
// where in the Value tree the Explorer is. Unlike the Pair, the Explorer
// provides functions that log errors any time a value access attempt is made
// and does not succeed. Imagine Value's as files and directories. Explorers are
// similar to the browser used to explore those files and directories.

// Explorers should be used when deserializing data whereas Pairs should be used
// when serializing data.

struct Explorer
{
  Explorer(const Pair* root);

  template<typename T>
  T As() const;
  template<typename T>
  T As(const T& defaultValue) const;

  std::string Path() const;
  const std::string& Key() const;
  int Size() const;
  bool Valid() const;

  Explorer operator()(const std::string& key) const;
  Explorer operator()(int index) const;
  Explorer operator[](int index) const;

private:
  Explorer(const Explorer* parent);
  Explorer(const Explorer* parent, const Pair* pair);
  Explorer(const Explorer* parent, const Value* value, int index);

  const Explorer* mParent;
  const Value* mValue;
  int mValueIndex;
  constexpr static int smInvalidValueIndex = -1;
};

template<typename T>
T Explorer::As() const
{
  LogAbortIf(
    !Valid(),
    "As can only be used on invalid Explorers if a default value is provided.");
  return mValue->As<T>();
}

template<typename T>
T Explorer::As(const T& defaultValue) const
{
  if (!Valid())
  {
    return defaultValue;
  }
  return mValue->As<T>();
}

} // namespace Vlk

#endif
