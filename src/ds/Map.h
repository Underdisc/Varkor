#ifndef ds_Map_h
#define ds_Map_h

#include "RbTree.h"
#include "ds/KvPair.h"

namespace Ds {

template<typename K, typename V>
class Map: public RbTree<KvPair<K, V>> {
public:
  typedef typename RbTree<KvPair<K, V>>::Iter Iter;
  typedef typename RbTree<KvPair<K, V>>::CIter CIter;
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
  V* TryGet(const K& key) const;
  V& Get(const K& key) const;
  bool Contains(const K& key) const;
};

} // namespace Ds

#include "Map.hh"

#endif
