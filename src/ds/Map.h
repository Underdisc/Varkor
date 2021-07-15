#ifndef ds_Map_h
#define ds_Map_h

#include "RbTree.h"

namespace Ds {

template<typename K, typename V>
class Map;

template<typename K, typename V>
struct KvPair
{
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

  friend Map<K, V>;
  friend RbTree<KvPair<K, V>>;
};

template<typename K, typename V>
class Map: public RbTree<KvPair<K, V>>
{
public:
  typedef RbTree<KvPair<K, V>>::Iter Iter;
  typedef RbTree<KvPair<K, V>>::CIter CIter;
  Iter begin() const;
  Iter end() const;
  CIter cbegin() const;
  CIter cend() const;

public:
  V& Insert(const K& key, const V& value);
  V& Insert(const K& key, V&& value);
  template<typename... Args>
  V& Emplace(const K& key, Args&&... args);
  void Remove(const K& key);
  V* Find(const K& key) const;
  V& Get(const K& key) const;
  bool Contains(const K& key) const;
};

} // namespace Ds

#include "Map.hh"

#endif
