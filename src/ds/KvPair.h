#ifndef ds_KvPair_h
#define ds_KvPair_h

namespace Ds {

template<typename T>
struct RbTree;
template<typename K, typename V>
struct Map;
template<typename T>
struct HashSet;
template<typename K, typename V>
struct HashMap;
template<typename T>
struct Vector;

template<typename K, typename V>
struct KvPair {
  const K& Key() const;
  V mValue;

private:
  K mKey;

  KvPair(const K& key, const V& value);
  KvPair(const K& key, V&& value);
  template<typename... Args>
  KvPair(const K& key, Args&&... args);
  bool operator>(const KvPair& other) const;
  bool operator<(const KvPair& other) const;
  bool operator>(const K& otherKey) const;
  bool operator<(const K& otherKey) const;
  bool operator==(const KvPair& other) const;
  bool operator!=(const KvPair& other) const;

  friend bool operator==(const KvPair<K, V>& kvPair, const K& key);
  friend bool operator!=(const KvPair<K, V>& kvPair, const K& key);
  friend bool operator==(const K& key, const KvPair<K, V>& kvPair);
  friend bool operator!=(const K& key, const KvPair<K, V>& kvPair);

  friend RbTree<KvPair<K, V>>;
  friend Map<K, V>;
  friend HashSet<KvPair<K, V>>;
  friend HashMap<K, V>;
  friend Vector<KvPair<K, V>>;
};

template<typename K, typename V>
bool operator==(const KvPair<K, V>& kvPair, const K& otherKey);
template<typename K, typename V>
bool operator!=(const KvPair<K, V>& kvPair, const K& otherKey);
template<typename K, typename V>
bool operator==(const K& otherKey, const KvPair<K, V>& kvPair);
template<typename K, typename V>
bool operator!=(const K& otherKey, const KvPair<K, V>& kvPair);

} // namespace Ds

#include "ds/KvPair.hh"

#endif
