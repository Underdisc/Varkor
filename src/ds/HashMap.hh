namespace Ds {

template<typename K, typename V>
inline size_t Hash(const KvPair<K, V>& kvPair) {
  return Hash(kvPair.Key());
}

template<typename K, typename V>
inline typename HashMap<K, V>::Iter HashMap<K, V>::Insert(
  const K& key, const V& value) {
  return HashSet<KvPair<K, V>>::Insert(KvPair<K, V>(key, value));
}

template<typename K, typename V>
inline typename HashMap<K, V>::Iter HashMap<K, V>::Insert(
  const K& key, V&& value) {
  return HashSet<KvPair<K, V>>::Insert(KvPair<K, V>(key, std::move(value)));
}

template<typename K, typename V>
inline typename HashMap<K, V>::Iter HashMap<K, V>::Remove(const K& key) {
  return HashSet<KvPair<K, V>>::Remove(key);
}

template<typename K, typename V>
inline typename HashMap<K, V>::Iter HashMap<K, V>::Remove(const CIter& it) {
  return HashSet<KvPair<K, V>>::Remove(it);
}

template<typename K, typename V>
inline typename HashMap<K, V>::Iter HashMap<K, V>::Find(const K& key) const {
  return HashSet<KvPair<K, V>>::Find(key);
}

template<typename K, typename V>
inline bool HashMap<K, V>::Contains(const K& key) const {
  return HashSet<KvPair<K, V>>::Contains(key);
}

} // namespace Ds
