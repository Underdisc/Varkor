#include <iostream>
#include <string>

#include "debug/MemLeak.h"
#include "vlk/Tokenizer.h"

std::ostream& operator<<(std::ostream& os, Vlk::Token::Type tokenType)
{
  switch (tokenType)
  {
  case Vlk::Token::Type::Invalid: os << "Invalid"; break;
  case Vlk::Token::Type::Key: os << "Key"; break;
  case Vlk::Token::Type::Value: os << "Value"; break;
  case Vlk::Token::Type::Whitespace: os << "Whitespace"; break;
  case Vlk::Token::Type::OpenBracket: os << "OpenBracket"; break;
  case Vlk::Token::Type::CloseBracket: os << "CloseBracket"; break;
  case Vlk::Token::Type::OpenBrace: os << "OpenBrace"; break;
  case Vlk::Token::Type::CloseBrace: os << "CloseBrace"; break;
  case Vlk::Token::Type::Comma: os << "Comma"; break;
  }
  return os;
}

void PrintTokens(const char* text)
{
  // Tokenize the text and print the results.
  Vlk::TokenizeResult result = Vlk::Tokenize(text);
  if (!result.Success())
  {
    std::cout << result.mError << std::endl;
    return;
  }
  for (const Vlk::Token& token : result.mTokens)
  {
    std::string tokenText(token.mText, token.mLength);
    std::cout << token.mType << ": " << tokenText << std::endl;
  }
}

void Key()
{
  std::cout << "<= Key =>" << std::endl;
  PrintTokens(
    ":a_key:\n"
    ":a_key_with_1234:\n"
    ":_a_key_with_a_starting_underscore:\n"
    ":A_KEY_WITH_CAPS:\n"
    ":_a_key_with_SOME_CAPS:\n");
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
  PrintTokens("[]{},");
  std::cout << std::endl;
}

void InvalidToken()
{
  std::cout << "<= InvalidToken =>" << std::endl;
  PrintTokens("+");
  PrintTokens("\"an incomplete string");
  PrintTokens(":an incomplete key");
}

int main()
{
  InitMemLeakOutput();
  Key();
  Value();
  Characters();
  InvalidToken();
}
