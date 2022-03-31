#include <iostream>

#include "Error.h"
#include "vlk/Explorer.h"

namespace Vlk {

Explorer::Explorer(const Value& rootVal):
  mParent(nullptr), mValue(&rootVal), mIsPair(false)
{}

Explorer::Explorer(const Explorer* parent): mParent(parent), mValue(nullptr) {}

Explorer::Explorer(const Explorer* parent, const Pair* pair):
  mParent(parent), mValue(pair), mIsPair(true)
{}

Explorer::Explorer(const Explorer* parent, const Value* value, size_t index):
  mParent(parent), mValue(value), mIsPair(false), mIndex(index)
{}

std::string Explorer::Path() const
{
  LogAbortIf(!Valid(), "Path of an invalid Explorer cannot be obtained.");

  // We collect all the ancestors in a vector so this Explorer's lineage can be
  // visited starting at the root. This Explorer is also considered an ancestor.
  Ds::Vector<const Explorer*> ancestors;
  const Explorer* current = this;
  while (current->mParent != nullptr) {
    ancestors.Push(current);
    current = current->mParent;
  }

  // Create the path by traversing this Explorer's lineage.
  std::stringstream path;
  path << "{}";
  while (!ancestors.Empty()) {
    const Explorer& ancestor = *ancestors.Top();
    if (ancestor.mIsPair) {
      const Pair* pair = (Pair*)ancestor.mValue;
      path << "{" << pair->Key() << "}";
    }
    else {
      path << "[" << ancestor.mIndex << "]";
    }
    ancestors.Pop();
  }
  return path.str();
}

const std::string& Explorer::Key() const
{
  LogAbortIf(!Valid(), "Key of an invalid Explorer cannot be optained.");
  if (!mIsPair) {
    std::stringstream error;
    error << Path() << " is not a Pair.";
    LogAbort(error.str().c_str());
  }
  const Pair& pair = *(Pair*)mValue;
  return pair.Key();
}

size_t Explorer::Size() const
{
  if (!Valid()) {
    return 0;
  }
  return mValue->Size();
}

bool Explorer::Valid() const
{
  return mValue != nullptr;
}

Explorer Explorer::operator()(const std::string& key) const
{
  if (!Valid()) {
    return Explorer(this);
  }
  const Pair* pair = mValue->TryGetPair(key);
  if (pair == nullptr) {
    std::stringstream error;
    error << Path() << " did not contain Pair with key :" << key << ":";
    LogError(error.str().c_str());
    return Explorer(this);
  }
  return Explorer(this, pair);
}

Explorer Explorer::operator()(size_t index) const
{
  if (!Valid()) {
    return Explorer(this);
  }
  const Pair* pair = mValue->TryGetPair(index);
  if (pair == nullptr) {
    std::stringstream error;
    error << Path() << " did not contain Pair at (" << index << ")";
    LogError(error.str().c_str());
    return Explorer(this);
  }
  return Explorer(this, pair);
}

Explorer Explorer::operator[](size_t index) const
{
  if (!Valid()) {
    return Explorer(this);
  }
  const Value* value = mValue->TryGetValue(index);
  if (value == nullptr) {
    std::stringstream error;
    error << Path() << " did not contain Value at [" << index << "]";
    LogError(error.str().c_str());
    return Explorer(this);
  }
  return Explorer(this, value, index);
}

} // namespace Vlk
