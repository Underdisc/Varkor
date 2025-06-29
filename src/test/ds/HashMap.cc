#include "ds/HashMap.h"
#include "Error.h"
#include "debug/MemLeak.h"
#include "test/ds/Print.h"
#include "test/ds/Test.h"

template<typename K, typename V>
Ds::HashMap<K, V> CreateHashMap(
  const Ds::Vector<K>& keys, const Ds::Vector<V>& values) {
  Assert(keys.Size() == values.Size());
  Ds::HashMap<K, V> hashMap;
  for (size_t i = 0; i < keys.Size(); ++i) {
    hashMap.Insert(keys[i], values[i]);
  }
  return hashMap;
}

void InsertRemoveFindContains() {
  // clang-format off
  Ds::Vector<std::string> strings = {"must", "klar", "wong", "sharp", "mode",
    "smith", "leben", "wann", "ziemlich", "quite"};
  Ds::Vector<int> ints = {0, 40, 15, 5, 25, 6, 3, 43, 9, 69};
  // clang-format on

  Ds::HashMap<std::string, int> stringIntMap = CreateHashMap(strings, ints);
  Ds::HashMap<int, std::string> intStringMap = CreateHashMap(ints, strings);
  std::cout << "<Initial Maps>\n"
            << stringIntMap << '\n'
            << intStringMap << '\n';

  Ds::Vector<const char*> stringRemoves = {"must", "ziemlich", "leben"};
  Ds::Vector<int> intRemoves = {0, 9, 3};
  for (size_t i = 0; i < stringRemoves.Size(); ++i) {
    stringIntMap.Remove(stringRemoves[i]);
    intStringMap.Remove(intRemoves[i]);
  }
  std::cout << "<Elements Removed>\n"
            << stringIntMap << '\n'
            << intStringMap << '\n';

  Ds::Vector<const char*> searches = {"must", "klar", "wann", "leben"};
  for (size_t i = 0; i < searches.Size(); ++i) {
    Ds::HashMap<std::string, int>::CIter it = stringIntMap.Find(searches[i]);
    if (it == stringIntMap.cend()) {
      std::cout << searches[i] << " not found, "
                << stringIntMap.Contains(searches[i]) << '\n';
    }
    else {
      std::cout << searches[i] << " found, "
                << stringIntMap.Contains(searches[i]) << '\n';
    }
  }
}

void Remove() {}

int main(void) {
  Error::Init();
  EnableLeakOutput();
  RunDsTest(InsertRemoveFindContains);
}
