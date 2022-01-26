#include <iostream>

#include "vlk/Explorer.h"
#include "vlk/Value.h"

void Deserialize()
{
  std::cout << "<= Deserialize =>\n";
  Vlk::Value rootVal;
  rootVal.Read("../vlk_Value/SerializeDeserialize.vlk");
  Vlk::Explorer rootEx(rootVal);
  Vlk::Explorer stringsEx = rootEx("Container")("Strings");

  // Test access of pairs and values that do not exist.
  rootEx(10)(0)("Invalid");
  stringsEx[4];
  stringsEx[0][3][0];

  // Test other functions supplied by the explorer.
  std::cout << "Invalid Size: " << rootEx("Invalid").Size() << "\n";
  std::cout << "PairArray Size: " << rootEx(3).Size() << "\n";
  std::cout << "PairArray Key: " << rootEx(3).Key() << "\n";
  std::cout << "Other Key: " << rootEx("ArrayOfPairArrays")[1](1).Key() << "\n";
  std::cout << std::endl;
}

int main()
{
  Deserialize();
}
