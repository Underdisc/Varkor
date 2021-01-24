#ifndef ds_Map_h
#define ds_Map_h

#include "RbTree.h"

namespace Ds {

template<typename K, typename V>
struct KvPair
{
  KvPair(const K& key, const V& value);
  KvPair(const K& key, V&& value);
  bool operator>(const KvPair& other) const;
  bool operator<(const KvPair& other) const;
  bool operator>(const K& otherKey) const;
  bool operator<(const K& otherKey) const;
  K mKey;
  V mValue;
};

template<typename K, typename V>
class Map: public RbTree<KvPair<K, V>>
{
public:
  void Insert(const K& key, const V& value);
  void Insert(const K& key, V&& value);
  void Remove(const K& key);
  V* Find(const K& key);
  bool Contains(const K& key);
};

} // namespace Ds

#include "Map.hh"

#endif
