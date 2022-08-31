#include <fstream>
#include <iostream>
#include <utility>

#include "debug/MemLeak.h"
#include "test/vlk/Util.h"
#include "vlk/Value.h"

void PrintParseError(const char* text)
{
  Vlk::Value rootVal;
  Result result = rootVal.Parse(text);
  std::cout << result.mError << std::endl;
}

void TrueValue()
{
  std::cout << "<= TrueValue =>" << std::endl;
  Vlk::Value rootVal;
  rootVal(std::string("Integer")) = 10;
  rootVal(std::string("Float")) = 10.5f;
  rootVal(std::string("String")) = "oooweee";
  PrintParsePrint(rootVal);
}

void ValueArray()
{
  std::cout << "<= ValueArray =>" << std::endl;
  Vlk::Value rootVal;
  Vlk::Value& intergerArrayVal = rootVal("Integers")[{5}];
  for (size_t i = 0; i < 5; ++i) {
    intergerArrayVal[i] = i;
  }

  Vlk::Value& floatArraysVal = rootVal("Floats")[{4, 5}];
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 5; ++j) {
      floatArraysVal[i][j] = (float)i + (float)j * 0.1f;
    }
  }

  Vlk::Value& stringArraysVal = rootVal("Strings")[{8}];
  for (size_t i = 0; i < 8; ++i) {
    stringArraysVal[i][{i}];
    for (size_t j = 0; j < i; ++j) {
      std::stringstream ss;
      ss << "string " << i << " " << j;
      stringArraysVal[i][j] = ss.str().c_str();
    }
  }
  PrintParsePrint(rootVal);
}

void PairArray()
{
  std::cout << "<= PairArray =>" << std::endl;
  Vlk::Value rootVal;
  rootVal("Integer") = 5;
  Vlk::Value& array1Val = rootVal("Array1");
  array1Val("Float") = 5.5f;
  Vlk::Value& array2Val = array1Val("Array2");
  Vlk::Value& stringArrayVal = array2Val("Strings")[{5}];
  for (size_t i = 0; i < 5; ++i) {
    std::stringstream ss;
    ss << "string " << i;
    stringArrayVal[i] = ss.str().c_str();
  }
  rootVal("EmptyArray");
  PrintParsePrint(rootVal);
}

void MultidimensionalArrays()
{
  std::cout << "<= MultidimensionalArrays =>" << std::endl;
  Vlk::Value rootVal;
  Vlk::Value& valueArraysVal = rootVal("ValueArrays");
  Vlk::Value& float1DVal = valueArraysVal("Float1D")[{4}];
  for (size_t i = 0; i < 4; ++i) {
    float1DVal[i] = (float)i;
  }
  Vlk::Value& float2DVal = valueArraysVal("Float2D")[{4}];
  for (size_t i = 0; i < 4; ++i) {
    float2DVal[i][{i + 5}];
    for (size_t j = 0; j < i + 5; ++j) {
      float2DVal[i][j] = (float)(j);
    }
  }
  Vlk::Value& float3DVal = valueArraysVal("Float3D")[{4, 10, 3}];
  for (size_t i = 0; i < 4; ++i) {
    for (size_t j = 0; j < 10; ++j) {
      for (size_t k = 0; k < 3; ++k) {
        float3DVal[i][j][k] = (float)(i + j + k);
      }
    }
  }
  PrintParsePrint(rootVal);
}

void ValueArrayPairArray()
{
  std::cout << "<= ValueArrayPairArray =>\n";
  Vlk::Value rootVal;
  Vlk::Value& valueArrayVal = rootVal("ValueArray")[{5}];
  for (int i = 0; i < 5; ++i) {
    valueArrayVal[i]("PairA") = "ValueA";
    valueArrayVal[i]("PairB") = "ValueB";
  }
  PrintParsePrint(rootVal);
}

void SerializeDeserialize()
{
  std::cout << "<= SerializeDeserialize =>" << std::endl;
  const char* filename = "SerializeDeserialize.vlk";
  // Serialization
  {
    Vlk::Value rootVal;
    rootVal("Integer") = 5;
    Vlk::Value& floats = rootVal("Floats")[{3}];
    floats[0] = 1;
    floats[1] = 11.34f;
    floats[2] = -3.1415f;

    Vlk::Value& containerVal = rootVal("Container");
    containerVal("SentenceString") = "This is a longer test string.";
    Vlk::Value& stringsVal = containerVal("Strings")[{3}];
    stringsVal[0][{2}];
    stringsVal[0][0] = "oh";
    stringsVal[0][1] = std::string("look");
    stringsVal[1][{2}];
    stringsVal[1][0] = "a";
    stringsVal[1][1] = std::string("stringy");
    stringsVal[2][{1}];
    stringsVal[2][0] = "thingy";

    Vlk::Value& pairArrayVal = rootVal("PairArray");
    pairArrayVal("Key0") = 0;
    pairArrayVal("Key1") = 1;
    pairArrayVal("Key2") = 2;
    pairArrayVal("Key3") = 3;

    Vlk::Value& arrayOfPairArraysVal = rootVal("ArrayOfPairArrays")[{2}];
    for (size_t i = 0; i < 2; ++i) {
      arrayOfPairArraysVal[i]("Key0") = 0;
      arrayOfPairArraysVal[i]("Key1") = 1;
    }
    Result result = rootVal.Write(filename);
    LogAbortIf(!result.Success(), result.mError.c_str());
  }
  // Deserialization
  {
    Vlk::Value rootVal;
    Result result = rootVal.Read(filename);
    LogAbortIf(!result.Success(), result.mError.c_str());
    const Vlk::Pair& integer = *rootVal.TryGetPair("Integer");
    std::cout << integer.Key() << ": " << integer.As<int>() << std::endl;
    const Vlk::Pair& floats = *rootVal.TryGetPair("Floats");
    std::cout << floats.Key() << ": " << std::endl;
    for (size_t i = 0; i < floats.Size(); ++i) {
      std::cout << "  " << floats.TryGetConstValue(i)->As<float>() << std::endl;
    }

    const Vlk::Pair& container = *rootVal.TryGetPair("Container");
    const Vlk::Pair& sentenceString =
      *container.TryGetConstPair("SentenceString");
    std::cout << sentenceString.Key() << ": "
              << sentenceString.As<std::string>() << std::endl;
    const Vlk::Pair& strings = *container.TryGetConstPair("Strings");
    std::cout << strings.Key() << ": " << std::endl;
    for (size_t i = 0; i < strings.Size(); ++i) {
      for (size_t j = 0; j < strings[i].Size(); ++j) {
        std::cout << "  " << strings[i][j].As<std::string>() << std::endl;
      }
    }

    const Vlk::Pair& pairArray = *rootVal.TryGetPair("PairArray");
    std::cout << pairArray.Key() << ": " << std::endl;
    for (size_t i = 0; i < pairArray.Size(); ++i) {
      std::cout << "  " << pairArray.TryGetConstPair(i)->Key() << ": "
                << pairArray.TryGetConstPair(i)->As<int>() << std::endl;
    }

    const Vlk::Pair& arrayOfPairArrays =
      *rootVal.TryGetPair("ArrayOfPairArrays");
    std::cout << arrayOfPairArrays.Key() << ":\n";
    for (size_t i = 0; i < arrayOfPairArrays.Size(); ++i) {
      const Vlk::Value& elementPairArray = arrayOfPairArrays[i];
      std::cout << "  PairArray" << i << ":\n";
      for (size_t j = 0; j < elementPairArray.Size(); ++j) {
        std::cout << "    " << elementPairArray.TryGetConstPair(j)->Key()
                  << ": " << elementPairArray.TryGetConstPair(j)->As<int>()
                  << "\n";
      }
    }
  }
}

Vlk::Value CreateTestValue()
{
  srand(0);
  Vlk::Value rootVal;
  rootVal("TrueValue") = "Value";
  Vlk::Value& valArrayVal = rootVal("ValueArray")[{5}];
  for (int i = 0; i < 5; ++i) {
    std::string value = "Value-";
    value += (char)('A' + i);
    valArrayVal[i] = value;
  }
  Vlk::Value& pairArrayVal = rootVal("PairArray");
  for (int i = 0; i < 5; ++i) {
    std::string key = "Pair-";
    key += (char)('A' + i);
    std::string value = "Value-";
    value += (char)('A' + i);
    pairArrayVal(key) = value;
  }
  return rootVal;
}

void Move()
{
  std::cout << "<= Move =>\n";
  Vlk::Value rootVal = CreateTestValue();

  Vlk::Value movedVal(std::move(rootVal("TrueValue")));
  std::cout << "<- MovedTrueValue ->\n" << movedVal << "\n";

  movedVal = std::move(rootVal("ValueArray"));
  movedVal[1] = "Replaced";
  movedVal[3] = "Replaced";
  std::cout << "<- MovedValueArray ->\n" << movedVal << "\n";

  movedVal = std::move(rootVal("PairArray"));
  movedVal("Pair-A") = "Replaced";
  std::cout << "<- MovedPairArray ->\n" << movedVal << "\n";
  std::cout << "<- Original ->\n" << rootVal << "\n";
}

void Errors()
{
  std::cout << "<= Errors =>" << std::endl;
  PrintParseError(".");
  PrintParseError("}");
  PrintParseError("{");
  PrintParseError("{:NoValue:");
  PrintParseError("{:ValueArray:[]]");
  PrintParseError("{:ValueArray:[]{}}");
  PrintParseError("{:TokenError");
  PrintParseError("{:ValidToken:\'Invalid:Token");

  // This tests line number information for parser errors.
  Vlk::Value rootVal;
  Result result = rootVal.Read("ParserError.vlk");
  std::cout << result.mError << std::endl;
}

void FindPair()
{
  std::cout << "<= FindPair =>\n";
  Vlk::Value rootVal = CreateTestValue();
  rootVal("TrueValue") = "Replaced";
  rootVal("ValueArray")[1] = "Replaced";
  rootVal("ValueArray")[3] = "Replaced";
  rootVal("PairArray")("Pair-C") = "Replaced";
  std::cout << rootVal << "\n";
}

void Copy()
{
  std::cout << "<= Copy =>\n";
  srand(0);

  Vlk::Value rootVal = CreateTestValue();
  Vlk::Value rootValCopy(rootVal);
  rootValCopy("TrueValue") = "Replaced";
  rootValCopy("ValueArray")[3] = "Replaced";
  rootValCopy("PairArray")("Pair-B") = "Replaced";
  std::cout << "<- Copy ->\n" << rootValCopy << "\n";

  rootValCopy = rootVal("TrueValue");
  std::cout << "<- TrueValueOverwrite ->\n" << rootValCopy << "\n";

  rootValCopy = rootVal("ValueArray");
  rootValCopy[0] = "Replaced";
  rootValCopy[4] = "Replaced";
  std::cout << "<- ValueArrayOverwrite ->\n" << rootValCopy << "\n";

  rootValCopy = rootVal("PairArray");
  rootValCopy("Pair-A") = "Replaced";
  rootValCopy("Pair-E") = "Replaced";
  std::cout << "<- PairArrayOverwrite ->\n" << rootValCopy << "\n";

  std::cout << "<- Original ->\n" << rootVal << "\n";
}

void Comparison()
{
  std::cout << "<= Comparison =>\n";
  auto compare = [](const Vlk::Value& a, const Vlk::Value& b)
  {
    std::cout << "==:" << (a == b) << " | !=:" << (a != b) << '\n';
  };
  Vlk::Value rootVal = CreateTestValue();

  std::cout << "<- Equal ->\n";
  Vlk::Value rootValCopy = rootVal;
  compare(rootVal, rootValCopy);

  std::cout << "<- DifferentType ->\n";
  Vlk::Value differentType;
  differentType = "Filler";
  compare(rootVal, differentType);

  std::cout << "<- DifferentTrueValue ->\n";
  rootValCopy = rootVal;
  rootValCopy("TrueValue") = "Replaced";
  compare(rootVal, rootValCopy);

  std::cout << "<- DifferentValueArraySize ->\n";
  rootValCopy = rootVal;
  rootValCopy("ValueArray").PushValue("Filler");
  compare(rootVal, rootValCopy);

  std::cout << "<- DifferentValueArray ->\n";
  rootValCopy = rootVal;
  rootValCopy("ValueArray")[2] = "Replaced";
  compare(rootVal, rootValCopy);

  std::cout << "<- DifferentPairArraySize ->\n";
  rootValCopy = rootVal;
  rootValCopy("PairArray")("Pair-F") = "Filler";
  compare(rootVal, rootValCopy);

  std::cout << "<- DifferentPairArrayValue ->\n";
  rootValCopy = rootVal;
  rootValCopy("PairArray")("Pair-B") = "Replaced";
  compare(rootVal, rootValCopy);

  std::cout << "<- DifferentKey ->\n";
  rootValCopy = rootVal;
  Vlk::Value& pairArrayVal = rootValCopy("PairArray");
  pairArrayVal.TryRemovePair("Pair-A");
  pairArrayVal("Pair-F") = "Value-F";
  compare(rootVal, rootValCopy);
}

void RunTest(void (*test)())
{
  static bool firstTest = true;
  if (firstTest) {
    firstTest = false;
  }
  else {
    std::cout << '\n';
  }
  test();
}

int main()
{
  EnableLeakOutput();
  RunTest(TrueValue);
  RunTest(ValueArray);
  RunTest(PairArray);
  RunTest(MultidimensionalArrays);
  RunTest(ValueArrayPairArray);
  RunTest(SerializeDeserialize);
  RunTest(Move);
  RunTest(Errors);
  RunTest(FindPair);
  RunTest(Copy);
  RunTest(Comparison);
}
