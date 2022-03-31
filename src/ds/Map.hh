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
template<typename... Args>
KvPair<K, V>::KvPair(const K& key, Args&&... args): mKey(key), mValue(args...)
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
const K& KvPair<K, V>::Key() const
{
  return mKey;
}

template<typename K, typename V>
typename Map<K, V>::Iter Map<K, V>::begin() const
{
  return RbTree<KvPair<K, V>>::begin();
}

template<typename K, typename V>
typename Map<K, V>::Iter Map<K, V>::end() const
{
  return RbTree<KvPair<K, V>>::end();
}

template<typename K, typename V>
typename Map<K, V>::CIter Map<K, V>::cbegin() const
{
  return RbTree<KvPair<K, V>>::cbegin();
}

template<typename K, typename V>
typename Map<K, V>::CIter Map<K, V>::cend() const
{
  return RbTree<KvPair<K, V>>::cend();
}

template<typename K, typename V>
V& Map<K, V>::Insert(const K& key, const V& value)
{
  Node* newNode = alloc Node(key, value);
  InsertNode(newNode);
  return newNode->mValue.mValue;
}

template<typename K, typename V>
V& Map<K, V>::Insert(const K& key, V&& value)
{
  Node* newNode = alloc Node(key, Util::Forward(value));
  InsertNode(newNode);
  return newNode->mValue.mValue;
}

template<typename K, typename V>
template<typename... Args>
V& Map<K, V>::Emplace(const K& key, Args&&... args)
{
  Node* newNode = alloc Node(key, args...);
  InsertNode(newNode);
  return newNode->mValue.mValue;
}

template<typename K, typename V>
void Map<K, V>::Remove(const K& key)
{
  Node* node = FindNode<K>(key);
  RemoveNode(node);
}

template<typename K, typename V>
V* Map<K, V>::Find(const K& key) const
{
  Node* node = FindNode<K>(key);
  if (node == nullptr) {
    return nullptr;
  }
  return &node->mValue.mValue;
}

template<typename K, typename V>
V& Map<K, V>::Get(const K& key) const
{
  V* value = Find(key);
  LogAbortIf(value == nullptr, "The Map did not contain the given key.");
  return *value;
}

template<typename K, typename V>
bool Map<K, V>::Contains(const K& key) const
{
  return FindNode<K>(key) != nullptr;
}

} // namespace Ds
