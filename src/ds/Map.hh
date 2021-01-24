#include "util/Utility.h"

namespace Ds {

template<typename K, typename V>
KvPair<K, V>::KvPair(const K& key, const V& value): mKey(key), mValue(value)
{}

template<typename K, typename V>
KvPair<K, V>::KvPair(const K& key, V&& value):
  mKey(key), mValue(Util::Forward(value))
{}

template<typename K, typename V>
bool KvPair<K, V>::operator>(const KvPair& other) const
{
  return mKey > other.mKey;
}

template<typename K, typename V>
bool KvPair<K, V>::operator<(const KvPair& other) const
{
  return mKey < other.mKey;
}

template<typename K, typename V>
bool KvPair<K, V>::operator>(const K& otherKey) const
{
  return mKey > otherKey;
}

template<typename K, typename V>
bool KvPair<K, V>::operator<(const K& otherKey) const
{
  return mKey < otherKey;
}

template<typename K, typename V>
void Map<K, V>::Insert(const K& key, const V& value)
{
  Emplace(key, value);
}

template<typename K, typename V>
void Map<K, V>::Insert(const K& key, V&& value)
{
  Emplace(key, Util::Forward(value));
}

template<typename K, typename V>
void Map<K, V>::Remove(const K& key)
{
  Node* node = FindNode<K>(key);
  RemoveNode(node);
}

template<typename K, typename V>
V* Map<K, V>::Find(const K& key)
{
  Node* node = FindNode<K>(key);
  if (node == nullptr)
  {
    return nullptr;
  }
  return &node->mValue.mValue;
}

template<typename K, typename V>
bool Map<K, V>::Contains(const K& key)
{
  return FindNode<K>(key) != nullptr;
}

} // namespace Ds
