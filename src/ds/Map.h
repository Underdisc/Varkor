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
  Iter Begin();
  Iter End();

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
