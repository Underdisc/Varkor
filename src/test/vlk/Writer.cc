#include <iostream>

#include "vlk/Writer.h"

void Value()
{
  std::cout << "<= Value =>" << std::endl;
  Vlk::Writer writer;
  // clang-format off
  writer << Request::Value << "Integer" << 10;
  writer << Request::Value << "Float" << 10.5f;
  writer << Request::Value << "String" << "10";
  // clang-format on
  const std::stringstream& content = writer.GetCompleteBuffer();
  std::cout << content.str() << std::endl;
}

void ValueArray()
{
  std::cout << "<= ValueArray =>" << std::endl;
  Vlk::Writer writer;

  writer << Request::ValueArray << "Integers";
  for (int i = 0; i < 5; ++i)
  {
    writer << i;
  }
  writer << Request::EndValueArray;

  writer << Request::ValueArray << "Floats";
  for (float i = 0.0f; i < 3.9f; i += 1.0f)
  {
    writer << Request::ValueArray;
    for (float j = i; j < (i + 0.5f - 0.05f); j += 0.1f)
    {
      writer << j;
    }
    writer << Request::EndValueArray;
  }
  writer << Request::EndValueArray;

  writer << Request::ValueArray << "Strings";
  for (int i = 0; i < 4; ++i)
  {
    writer << Request::ValueArray;
    for (int j = 0; j < i; ++j)
    {
      std::stringstream ss;
      ss << "string " << i << " " << j;
      writer << ss.str().c_str();
    }
    writer << Request::EndValueArray;
  }
  writer << Request::EndValueArray;

  const std::stringstream& content = writer.GetCompleteBuffer();
  std::cout << content.str() << std::endl;
}

void Array()
{
  std::cout << "<= Array =>" << std::endl;
  Vlk::Writer writer;
  writer << Request::Value << "Integer" << 5;
  writer << Request::Array << "Array1";
  writer << Request::Value << "Float" << 5.5f;
  writer << Request::Array << "Array2";
  writer << Request::ValueArray << "Strings";
  for (int i = 0; i < 5; ++i)
  {
    std::stringstream ss;
    ss << "string " << i;
    writer << ss.str().c_str();
  }
  writer << Request::EndValueArray << Request::EndArray << Request::EndArray;

  const std::stringstream& content = writer.GetCompleteBuffer();
  std::cout << content.str() << std::endl;
}

void Errors()
{
  std::cout << "<= Errors =>" << std::endl;
  Vlk::Writer writer;
  int errorNumber = 0;

  // Each of the incorrect sequences below will cause a crash.
  // clang-format off
  switch (errorNumber)
  {
  case 0: writer << "Error";
  case 1: writer << Request::Value << Request::Value;
  case 2: writer << Request::Value << "Key" << Request::Value;
  case 3: writer << Request::Value << "Key" << "Value" << "Error";
  case 4: writer << Request::EndValueArray;
  case 5: writer << Request::ValueArray << Request::Value;
  case 6: writer << Request::ValueArray << "Key" << Request::Value;
  case 7:
    writer << Request::ValueArray << "Key" << Request::EndValueArray
           << Request::Value << "Key" << "Value" << "Error";
  case 8:
    writer << Request::ValueArray << "Key" << "Value";
    writer.GetCompleteBuffer();
  case 9: writer << Request::ValueArray << Request::EndValueArray;
  case 10: writer << Request::EndArray;
  case 11: writer << Request::Array << Request::EndArray;
  case 12: writer << Request::Array << "Key" << Request::EndArray << "Error";
  case 13:
    writer << Request::Array << "Key" << Request::EndArray << Request::EndArray;
  case 14:
    writer << Request::Array << "Key";
    writer.GetCompleteBuffer();
  case 15: writer << Request::Array << Request::Array;
  case 16:
    writer << Request::Array << "Key" << Request::Array << "Key"
           << Request::EndArray;
    writer.GetCompleteBuffer();
  }
  // clang-format on
}

int main()
{
  Value();
  ValueArray();
  Array();
  //Errors();
}
