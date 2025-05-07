#ifndef ds_KvPair_h
#define ds_KvPair_h

namespace Ds {

template<typename T>
class RbTree;
template<typename K, typename V>
class Map;

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

  friend RbTree<KvPair<K, V>>;
  friend Map<K, V>;
};

} // namespace Ds

#include "ds/KvPair.hh"

#endif
