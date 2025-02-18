#include <iostream>

#include "test/Test.h"
#include "vlk/Explorer.h"
#include "vlk/Value.h"

void Deserialize() {
  Vlk::Value rootVal;
  rootVal.Read("../vlk_Value/SerializeDeserialize.vlk");
  Vlk::Explorer rootEx(rootVal);
  Vlk::Explorer containerEx = rootEx("Container");
  Vlk::Explorer stringsEx = containerEx("Strings");

  // Test access of pairs and values that do not exist.
  rootEx(10)(0)("Invalid");
  stringsEx[4];
  stringsEx[0][3][0];

  // Test other functions supplied by the explorer.
  size_t invalidSize = rootEx("Invalid").Size();
  std::cout << "Invalid Size: " << invalidSize << '\n';
  std::cout << "PairArray Size: " << rootEx(3).Size() << '\n';
  std::cout << "PairArray Key: " << rootEx(3).Key() << '\n';
  std::cout << "Other Key: " << rootEx("ArrayOfPairArrays")[1](1).Key() << '\n';
}

int main() {
  RunTest(Deserialize);
}
