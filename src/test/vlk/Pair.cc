#include <fstream>
#include <iostream>

#include "debug/MemLeak.h"
#include "vlk/Pair.h"

void PrintParsePrint(const Vlk::Value& rootVal)
{
  std::stringstream rootText;
  rootText << rootVal;
  Vlk::Value rootValCopy;
  rootValCopy.Parse(rootText.str().c_str());
  std::cout << rootValCopy << std::endl;
}

void PrintParseError(const char* text)
{
  Vlk::Value rootVal;
  Util::Result result = rootVal.Parse(text);
  std::cout << result.mError << std::endl;
}

void String()
{
  std::cout << "<= String =>" << std::endl;
  Vlk::Value rootVal;
  rootVal(std::string("Integer")) = 10;
  rootVal(std::string("Float")) = 10.5f;
  rootVal(std::string("String")) = "10";
  PrintParsePrint(rootVal);
  std::cout << std::endl;
}

void ValueArray()
{
  std::cout << "<= ValueArray =>" << std::endl;
  Vlk::Value rootVal;
  Vlk::Value& intergerArrayVal = rootVal("Integers")[{5}];
  for (size_t i = 0; i < 5; ++i)
  {
    intergerArrayVal[i] = i;
  }

  Vlk::Value& floatArraysVal = rootVal("Floats")[{4, 5}];
  for (size_t i = 0; i < 4; ++i)
  {
    for (size_t j = 0; j < 5; ++j)
    {
      floatArraysVal[i][j] = (float)i + (float)j * 0.1f;
    }
  }

  Vlk::Value& stringArraysVal = rootVal("Strings")[{8}];
  for (size_t i = 0; i < 8; ++i)
  {
    stringArraysVal[i][{i}];
    for (size_t j = 0; j < i; ++j)
    {
      std::stringstream ss;
      ss << "string " << i << " " << j;
      stringArraysVal[i][j] = ss.str().c_str();
    }
  }
  PrintParsePrint(rootVal);
  std::cout << std::endl;
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
  for (size_t i = 0; i < 5; ++i)
  {
    std::stringstream ss;
    ss << "string " << i;
    stringArrayVal[i] = ss.str().c_str();
  }
  rootVal("EmptyArray");
  PrintParsePrint(rootVal);
  std::cout << std::endl;
}

void MultidimensionalArrays()
{
  std::cout << "<= MultidimensionalArrays =>" << std::endl;
  Vlk::Value rootVal;
  Vlk::Value& valueArraysVal = rootVal("ValueArrays");
  Vlk::Value& float1DVal = valueArraysVal("Float1D")[{4}];
  for (size_t i = 0; i < 4; ++i)
  {
    float1DVal[i] = (float)i;
  }
  Vlk::Value& float2DVal = valueArraysVal("Float2D")[{4}];
  for (size_t i = 0; i < 4; ++i)
  {
    float2DVal[i][{i + 5}];
    for (size_t j = 0; j < i + 5; ++j)
    {
      float2DVal[i][j] = (float)(j);
    }
  }
  Vlk::Value& float3DVal = valueArraysVal("Float3D")[{4, 10, 3}];
  for (size_t i = 0; i < 4; ++i)
  {
    for (size_t j = 0; j < 10; ++j)
    {
      for (size_t k = 0; k < 3; ++k)
      {
        float3DVal[i][j][k] = (float)(i + j + k);
      }
    }
  }
  PrintParsePrint(rootVal);
  std::cout << std::endl;
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
    Util::Result result = rootVal.Write(filename);
    LogAbortIf(!result.Success(), result.mError.c_str());
  }
  // Deserialization
  {
    Vlk::Value rootVal;
    Util::Result result = rootVal.Read(filename);
    LogAbortIf(!result.Success(), result.mError.c_str());
    const Vlk::Pair& integer = *rootVal.TryGetPair("Integer");
    std::cout << integer.Key() << ": " << integer.As<int>() << std::endl;
    const Vlk::Pair& floats = *rootVal.TryGetPair("Floats");
    std::cout << floats.Key() << ": " << std::endl;
    for (size_t i = 0; i < floats.Size(); ++i)
    {
      std::cout << "  " << floats.TryGetValue(i)->As<float>() << std::endl;
    }
    const Vlk::Pair& strings =
      *rootVal.TryGetPair("Container")->TryGetPair("Strings");
    std::cout << strings.Key() << ": " << std::endl;
    for (size_t i = 0; i < strings.Size(); ++i)
    {
      for (size_t j = 0; j < strings[i].Size(); ++j)
      {
        std::cout << "  " << strings[i][j].As<std::string>() << std::endl;
      }
    }
    const Vlk::Pair& pairArray = *rootVal.TryGetPair("PairArray");
    std::cout << pairArray.Key() << ": " << std::endl;
    for (size_t i = 0; i < pairArray.Size(); ++i)
    {
      std::cout << "  " << pairArray.TryGetPair(i)->Key() << ": "
                << pairArray(i).As<int>() << std::endl;
    }
  }
  std::cout << std::endl;
}

void Move()
{
  std::cout << "<= Move =>" << std::endl;
  auto movePrint = [](Vlk::Value& val)
  {
    Vlk::Value movedVal(Util::Move(val));
    std::cout << movedVal << std::endl;
  };
  // Value
  {
    Vlk::Value rootVal;
    rootVal = 5;
    movePrint(rootVal);
  }
  // PairArray
  {
    Vlk::Value rootVal;
    for (size_t i = 0; i < 10; ++i)
    {
      std::stringstream ss;
      ss << "Key" << i;
      rootVal(ss.str()) = i;
    }
    movePrint(rootVal);
  }
  // ValueArray
  {
    Vlk::Value rootVal;
    rootVal[{10}];
    for (size_t i = 0; i < 10; ++i)
    {
      rootVal[i] = i;
    }
    movePrint(rootVal);
  }
  std::cout << std::endl;
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
  PrintParseError("{:ValidToken:\"Invalid:Token");

  // This tests line number information for parser errors.
  Vlk::Value rootVal;
  Util::Result result = rootVal.Read("ParserError.vlk");
  std::cout << result.mError << std::endl;
}

int main()
{
  InitMemLeakOutput();
  String();
  ValueArray();
  PairArray();
  MultidimensionalArrays();
  SerializeDeserialize();
  Move();
  Errors();
}
