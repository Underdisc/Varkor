#ifndef test_ds_Print_h
#define test_ds_Print_h

#include <iostream>

#include "ds/List.h"
#include "ds/Map.h"
#include "ds/RbTree.h"
#include "ds/Vector.h"

template<typename T>
void PrintVector(const Ds::Vector<T>& vector, bool stats = true);
template<typename T>
std::ostream& operator<<(std::ostream& os, const Ds::Vector<T>& vector);
template<typename T>
void PrintList(const Ds::List<T>& list);
template<typename T>
void PrintRbTree(const Ds::RbTree<T>& rbTree);
template<typename K, typename V>
void PrintMap(const Ds::Map<K, V>& map);

#include "Print.hh"

#endif
