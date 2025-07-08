namespace Ds {

template<typename K, typename V>
KvPair<K, V>::KvPair(const K& key, const V& value): mKey(key), mValue(value) {}

template<typename K, typename V>
KvPair<K, V>::KvPair(const K& key, V&& value):
  mKey(key), mValue(std::forward<V>(value)) {}

template<typename K, typename V>
template<typename... Args>
KvPair<K, V>::KvPair(const K& key, Args&&... args):
  mKey(key), mValue(args...) {}

template<typename K, typename V>
bool KvPair<K, V>::operator>(const KvPair& other) const {
  return mKey > other.mKey;
}

template<typename K, typename V>
bool KvPair<K, V>::operator<(const KvPair& other) const {
  return mKey < other.mKey;
}

template<typename K, typename V>
bool KvPair<K, V>::operator>(const K& otherKey) const {
  return mKey > otherKey;
}

template<typename K, typename V>
bool KvPair<K, V>::operator<(const K& otherKey) const {
  return mKey < otherKey;
}

template<typename K, typename V>
bool KvPair<K, V>::operator==(const KvPair<K, V>& other) const {
  return mKey == other.mKey;
}

template<typename K, typename V>
bool KvPair<K, V>::operator!=(const KvPair<K, V>& other) const {
  return mKey != other.mKey;
}

template<typename K, typename V>
const K& KvPair<K, V>::Key() const {
  return mKey;
}

template<typename K, typename V>
inline bool operator==(const KvPair<K, V>& kvPair, const K& key) {
  return kvPair.Key() == key;
}

template<typename K, typename V>
inline bool operator!=(const KvPair<K, V>& kvPair, const K& key) {
  return kvPair.Key() != key;
}

template<typename K, typename V>
inline bool operator==(const K& key, const KvPair<K, V>& kvPair) {
  return key == kvPair.Key();
}

template<typename K, typename V>
inline bool operator!=(const K& key, const KvPair<K, V>& kvPair) {
  return key != kvPair.Key();
}

} // namespace Ds
