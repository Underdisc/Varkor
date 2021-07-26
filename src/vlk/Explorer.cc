#include <iostream>

#include "Error.h"
#include "vlk/Explorer.h"

namespace Vlk {

Explorer::Explorer(const Pair* root):
  mParent(nullptr), mValue(root), mValueIndex(smInvalidValueIndex)
{}

Explorer::Explorer(const Explorer* parent):
  mParent(parent), mValue(nullptr), mValueIndex(smInvalidValueIndex)
{}

Explorer::Explorer(const Explorer* parent, const Pair* pair):
  mParent(parent), mValue(pair), mValueIndex(smInvalidValueIndex)
{}

Explorer::Explorer(const Explorer* parent, const Value* value, int valueIndex):
  mParent(parent), mValue(value), mValueIndex(valueIndex)
{}

std::string Explorer::Path() const
{
  LogAbortIf(!Valid(), "Path of an invalid value cannot be obtained");

  // We collect all the ancestors in a vector so this Explorer's lineage can be
  // visited starting at the root. This Explorer is also considered an ancestor.
  Ds::Vector<const Explorer*> ancestors;
  const Explorer* current = this;
  while (current != nullptr)
  {
    ancestors.Push(current);
    current = current->mParent;
  }

  // Create the path by traversing this Explorer's lineage.
  std::stringstream path;
  for (int i = ancestors.Size() - 1; i >= 0; --i)
  {
    const Explorer& ancestor = *ancestors[i];
    if (ancestor.mValueIndex == smInvalidValueIndex)
    {
      const Pair* pair = (Pair*)ancestor.mValue;
      path << ":" << pair->Key();
    } else
    {
      path << "[" << ancestor.mValueIndex << "]";
    }
  }
  return path.str();
}

const std::string& Explorer::Key() const
{
  LogAbortIf(
    mValueIndex != smInvalidValueIndex,
    "A key can only be retrieved from a Pair.");
  const Pair& pair = *(Pair*)mValue;
  return pair.Key();
}

int Explorer::Size() const
{
  if (!Valid())
  {
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
  if (!Valid())
  {
    return Explorer(this);
  }
  const Pair* pair = mValue->TryGetPair(key);
  if (pair == nullptr)
  {
    std::stringstream error;
    error << Path() << " did not contain Pair with key :" << key << ":";
    LogError(error.str().c_str());
    return Explorer(this);
  }
  return Explorer(this, pair);
}

Explorer Explorer::operator()(int index) const
{
  if (!Valid())
  {
    return Explorer(this);
  }
  const Pair* pair = mValue->TryGetPair(index);
  if (pair == nullptr)
  {
    std::stringstream error;
    error << Path() << " did not contain Pair at (" << index << ")";
    LogError(error.str().c_str());
    return Explorer(this);
  }
  return Explorer(this, pair);
}

Explorer Explorer::operator[](int index) const
{
  if (!Valid())
  {
    return Explorer(this);
  }
  const Value* value = mValue->TryGetValue(index);
  if (value == nullptr)
  {
    std::stringstream error;
    error << Path() << " did not contain Value at [" << index << "]";
    LogError(error.str().c_str());
    return Explorer(this);
  }
  return Explorer(this, value, index);
}

} // namespace Vlk
