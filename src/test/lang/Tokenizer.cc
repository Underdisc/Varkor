#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

#include "lang/Tokenizer.h"

std::ostream& operator<<(std::ostream& os, Lang::Token::Type tokenType)
{
  switch (tokenType)
  {
  case Lang::Token::Type::Invalid: os << "Invalid"; break;
  case Lang::Token::Type::Key: os << "Key"; break;
  case Lang::Token::Type::Value: os << "Value"; break;
  case Lang::Token::Type::Whitespace: os << "Whitespace"; break;
  case Lang::Token::Type::Colon: os << "Colon"; break;
  case Lang::Token::Type::OpenBracket: os << "OpenBracket"; break;
  case Lang::Token::Type::CloseBracket: os << "CloseBracket"; break;
  case Lang::Token::Type::OpenBrace: os << "OpenBrace"; break;
  case Lang::Token::Type::CloseBrace: os << "CloseBrace"; break;
  }
  return os;
}

void PrintTokens(const char* text)
{
  // Tokenize the text and print the results.
  Lang::Tokenizer tokenizer;
  Lang::Tokenizer::Result result = tokenizer.Tokenize(text);
  if (result.mError.size() > 0)
  {
    std::cout << result.mError << std::endl;
    return;
  }
  for (const Lang::Tokenizer::Token& token : result.mTokens)
  {
    std::string tokenText(token.mText, token.mLength);
    std::cout << token.mType << ": " << tokenText << std::endl;
  }
}

void Key()
{
  std::cout << "<= Key =>" << std::endl;
  PrintTokens(
    "a_key\n"
    "a_key_with_1234\n"
    "_a_key_with_a_starting_underscore\n"
    "A_KEY_WITH_CAPS\n"
    "_a_key_with_SOME_CAPS\n");
  std::cout << std::endl;
}

void Value()
{
  std::cout << "<= Value =>" << std::endl;
  PrintTokens(
    "1\n"
    "143\n"
    "1.0\n"
    "123.456\n"
    "\"AStringValue\"\n"
    "\"Another_?+-!\"\n");
  std::cout << std::endl;
}

void Characters()
{
  std::cout << "<= Characters =>" << std::endl;
  PrintTokens(":[]{}");
  std::cout << std::endl;
}

void Writer()
{
  // This test takes the output from the Writer test to ensure that the
  // Tokenizer can tokenize the text created by the Writer.
  std::cout << "<= Writer =>" << std::endl;
  constexpr int buffSize = 1024;
  char rawLine[buffSize];
  std::string text;
  std::ifstream fileStream("lang_Writer_out.txt");

  while (!fileStream.eof())
  {
    // Read an entire line from the stream into a string.
    fileStream.getline(rawLine, buffSize);
    std::string line(rawLine);
    while (fileStream.fail() && !fileStream.eof())
    {
      fileStream.getline(rawLine, 1024);
      line += rawLine;
    }

    // If the line has a test header, perform a test with any collected text and
    // display the next test header.
    if (line[0] == '<' && line[1] == '=')
    {
      if (text.size() > 0)
      {
        text += "\0";
        PrintTokens(text.data());
        text.clear();
        std::cout << std::endl;
      }
      size_t testNameEnd = line.find(' ', 3);
      std::cout << "<- " << line.substr(3, testNameEnd - 3) << " ->"
                << std::endl;
    } else
    {
      line += "\n";
      text += line;
    }
  }
  // Perform a test with the remaining collected text.
  text += "\0";
  PrintTokens(text.data());
  std::cout << std::endl;
}

void InvalidToken()
{
  std::cout << "<= InvalidToken =>" << std::endl;
  PrintTokens("an invalid character +");
  PrintTokens("\"an incomplete string");
  PrintTokens("an invalid value 1.");
}

int main()
{
  Key();
  Value();
  Characters();
  Writer();
  InvalidToken();
}
