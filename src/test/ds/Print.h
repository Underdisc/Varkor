#ifndef test_ds_Print_h
#define test_ds_Print_h

#include <iostream>

#include "ds/HashSet.h"
#include "ds/List.h"
#include "ds/Map.h"
#include "ds/Pool.h"
#include "ds/RbTree.h"
#include "ds/Vector.h"

template<typename T>
std::ostream& operator<<(std::ostream& os, const Ds::HashSet<T>& hashSet);
template<typename T>
std::ostream& operator<<(std::ostream& os, const Ds::Vector<T>& vector);
template<typename T>
void PrintHashSetDs(const Ds::HashSet<T>& hashSet);
template<typename T>
void PrintVector(const Ds::Vector<T>& vector, bool stats = true);
template<typename T>
void PrintList(const Ds::List<T>& list);
template<typename T>
void PrintRbTree(const Ds::RbTree<T>& rbTree);
template<typename K, typename V>
void PrintMap(const Ds::Map<K, V>& map);
template<typename T>
void PrintPool(const Ds::Pool<T>& pool);

#include "Print.hh"

#endif
