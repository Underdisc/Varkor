#include <utility>

namespace Ds {

template<typename K, typename V>
typename Map<K, V>::Iter Map<K, V>::begin() const {
  return RbTree<KvPair<K, V>>::begin();
}

template<typename K, typename V>
typename Map<K, V>::Iter Map<K, V>::end() const {
  return RbTree<KvPair<K, V>>::end();
}

template<typename K, typename V>
typename Map<K, V>::CIter Map<K, V>::cbegin() const {
  return RbTree<KvPair<K, V>>::cbegin();
}

template<typename K, typename V>
typename Map<K, V>::CIter Map<K, V>::cend() const {
  return RbTree<KvPair<K, V>>::cend();
}

template<typename K, typename V>
V& Map<K, V>::Insert(const K& key, const V& value) {
  typename Map<K, V>::Node* newNode =
    alloc typename Map<K, V>::Node(key, value);
  Map<K, V>::InsertNode(newNode);
  return newNode->mValue.mValue;
}

template<typename K, typename V>
V& Map<K, V>::Insert(const K& key, V&& value) {
  typename Map<K, V>::Node* newNode =
    alloc typename Map<K, V>::Node(key, std::forward<V>(value));
  Map<K, V>::InsertNode(newNode);
  return newNode->mValue.mValue;
}

template<typename K, typename V>
template<typename... Args>
V& Map<K, V>::Emplace(const K& key, Args&&... args) {
  typename Map<K, V>::Node* newNode =
    alloc typename Map<K, V>::Node(key, std::forward<Args>(args)...);
  Map<K, V>::InsertNode(newNode);
  return newNode->mValue.mValue;
}

template<typename K, typename V>
void Map<K, V>::Remove(const K& key) {
  typename Map<K, V>::Node* node = Map<K, V>::template FindNode<K>(key);
  Map<K, V>::RemoveNode(node);
}

template<typename K, typename V>
V* Map<K, V>::TryGet(const K& key) const {
  typename Map<K, V>::Node* node = Map<K, V>::template FindNode<K>(key);
  if (node == nullptr) {
    return nullptr;
  }
  return &node->mValue.mValue;
}

template<typename K, typename V>
V& Map<K, V>::Get(const K& key) const {
  V* value = TryGet(key);
  LogAbortIf(value == nullptr, "The Map did not contain the given key.");
  return *value;
}

template<typename K, typename V>
bool Map<K, V>::Contains(const K& key) const {
  return Map<K, V>::template FindNode<K>(key) != nullptr;
}

} // namespace Ds
