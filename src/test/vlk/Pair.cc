#include <fstream>
#include <iostream>

#include "debug/MemLeak.h"
#include "vlk/Pair.h"

void PrintParsePrint(const Vlk::Pair& root)
{
  std::stringstream rootText;
  rootText << root;
  Vlk::Pair rootCopy;
  rootCopy.Parse(rootText.str().c_str());
  std::cout << rootCopy << std::endl;
}

void PrintParseError(const char* text)
{
  Vlk::Pair root;
  Util::Result result = root.Parse(text);
  std::cout << result.mError << std::endl;
}

void String()
{
  std::cout << "<= String =>" << std::endl;
  Vlk::Pair root(std::string("RootKey"));
  root(std::string("Integer")) = 10;
  root(std::string("Float")) = 10.5f;
  root(std::string("String")) = "10";
  PrintParsePrint(root);
  std::cout << std::endl;
}

void ValueArray()
{
  std::cout << "<= ValueArray =>" << std::endl;
  Vlk::Pair root;
  Vlk::Value& intergerArray = root("Integers")[{5}];
  for (int i = 0; i < 5; ++i)
  {
    intergerArray[i] = i;
  }

  Vlk::Value& floatArrays = root("Floats")[{4, 5}];
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 5; ++j)
    {
      floatArrays[i][j] = (float)i + (float)j * 0.1f;
    }
  }

  Vlk::Value& stringArrays = root("Strings")[{8}];
  for (int i = 0; i < 8; ++i)
  {
    stringArrays[i][{i}];
    for (int j = 0; j < i; ++j)
    {
      std::stringstream ss;
      ss << "string " << i << " " << j;
      stringArrays[i][j] = ss.str().c_str();
    }
  }
  PrintParsePrint(root);
  std::cout << std::endl;
}

void PairArray()
{
  std::cout << "<= PairArray =>" << std::endl;
  Vlk::Pair root;
  root("Integer") = 5;
  Vlk::Pair& array1 = root("Array1");
  array1("Float") = 5.5f;
  Vlk::Pair& array2 = array1("Array2");
  Vlk::Value& stringArray = array2("Strings")[{5}];
  for (int i = 0; i < 5; ++i)
  {
    std::stringstream ss;
    ss << "string " << i;
    stringArray[i] = ss.str().c_str();
  }
  root("EmptyArray");
  PrintParsePrint(root);
  std::cout << std::endl;
}

void MultidimensionalArrays()
{
  std::cout << "<= MultidimensionalArrays =>" << std::endl;
  Vlk::Pair root;
  Vlk::Pair& valueArrays = root("ValueArrays");
  Vlk::Value& float1D = valueArrays("Float1D")[{4}];
  for (int i = 0; i < 4; ++i)
  {
    float1D[i] = (float)i;
  }
  Vlk::Value& float2D = valueArrays("Float2D")[{4}];
  for (int i = 0; i < 4; ++i)
  {
    float2D[i][{i + 5}];
    for (int j = 0; j < i + 5; ++j)
    {
      float2D[i][j] = (float)(j);
    }
  }
  Vlk::Value& float3D = valueArrays("Float3D")[{4, 10, 3}];
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 10; ++j)
    {
      for (int k = 0; k < 3; ++k)
      {
        float3D[i][j][k] = (float)(i + j + k);
      }
    }
  }
  PrintParsePrint(root);
  std::cout << std::endl;
}

void SerializeDeserialize()
{
  std::cout << "<= SerializeDeserialize =>" << std::endl;
  const char* filename = "SerializeDeserialize.vlk";
  // Serialization
  {
    Vlk::Pair root;
    Vlk::Pair& integer = root("Integer");
    integer = 5;
    Vlk::Pair& floats = root("Floats");
    floats[{3}];
    floats[0] = 1;
    floats[1] = 11.34f;
    floats[2] = -3.1415f;
    Vlk::Pair& container = root("Container");
    Vlk::Pair& strings = container("Strings");
    strings[{3}];
    strings[0][{2}];
    strings[0][0] = "oh";
    strings[0][1] = std::string("look");
    strings[1][{2}];
    strings[1][0] = "a";
    strings[1][1] = std::string("stringy");
    strings[2][{1}];
    strings[2][0] = "thingy";
    Vlk::Pair& pairArray = root("PairArray");
    pairArray("Key0") = 0;
    pairArray("Key1") = 1;
    pairArray("Key2") = 2;
    pairArray("Key3") = 3;
    Util::Result result = root.Write(filename);
    LogAbortIf(!result.Success(), result.mError.c_str());
  }
  // Deserialization
  {
    Vlk::Pair root;
    Util::Result result = root.Read(filename);
    LogAbortIf(!result.Success(), result.mError.c_str());
    const Vlk::Pair& integer = *root.TryGetPair("Integer");
    std::cout << integer.Key() << ": " << integer.As<int>() << std::endl;
    const Vlk::Pair& floats = *root.TryGetPair("Floats");
    std::cout << floats.Key() << ": " << std::endl;
    for (int i = 0; i < floats.Size(); ++i)
    {
      std::cout << "  " << floats.TryGetValue(i)->As<float>() << std::endl;
    }
    const Vlk::Pair& strings =
      *root.TryGetPair("Container")->TryGetPair("Strings");
    std::cout << strings.Key() << ": " << std::endl;
    for (int i = 0; i < strings.Size(); ++i)
    {
      for (int j = 0; j < strings[i].Size(); ++j)
      {
        std::cout << "  " << strings[i][j].As<std::string>() << std::endl;
      }
    }
    const Vlk::Pair& pairArray = *root.TryGetPair("PairArray");
    std::cout << pairArray.Key() << ": " << std::endl;
    for (int i = 0; i < pairArray.Size(); ++i)
    {
      std::cout << "  " << pairArray.TryGetPair(i)->Key() << ": "
                << pairArray(i).As<int>() << std::endl;
    }
  }
}

void Move()
{
  std::cout << "<= Move =>" << std::endl;
  auto movePrint = [](Vlk::Pair& pair)
  {
    Vlk::Pair movedPair(Util::Move(pair));
    std::cout << movedPair << std::endl;
  };
  // Value
  {
    Vlk::Pair root("Value");
    root = 5;
    movePrint(root);
  }
  // PairArray
  {
    Vlk::Pair root("PairArray");
    for (int i = 0; i < 10; ++i)
    {
      std::stringstream ss;
      ss << "Key" << i;
      root(ss.str()) = i;
    }
    movePrint(root);
  }
  // ValueArray
  {
    Vlk::Pair root("ValueArray");
    root[{10}];
    for (int i = 0; i < 10; ++i)
    {
      root[i] = i;
    }
    movePrint(root);
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
