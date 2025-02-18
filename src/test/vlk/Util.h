#ifndef test_vlk_Util_h
#define test_vlk_Util_h

#include <iostream>
#include <sstream>

#include "vlk/Value.h"

void PrintParsePrint(const Vlk::Value& rootVal) {
  std::stringstream rootText;
  rootText << rootVal;
  Vlk::Value rootValCopy;
  rootValCopy.Parse(rootText.str().c_str());
  std::cout << rootValCopy << std::endl;
}

#endif
