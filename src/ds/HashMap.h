#ifndef ds_HashMap_h
#define ds_HashMap_h

#include "ds/HashSet.h"
#include "ds/KvPair.h"

namespace Ds {

template<typename K, typename V>
size_t Hash(const KvPair<K, V>& kvPair);

template<typename K, typename V>
struct HashMap: public HashSet<KvPair<K, V>> {
  typedef typename HashSet<KvPair<K, V>>::Iter Iter;
  typedef typename HashSet<KvPair<K, V>>::CIter CIter;

  Iter Insert(const K& key, const V& value);
  Iter Insert(const K& key, V&& value);
  Iter Remove(const CIter& it);
  Iter Remove(const K& key);
  Iter Find(const K& key) const;
  bool Contains(const K& key) const;
};

} // namespace Ds

#include "ds/HashMap.hh"

#endif
