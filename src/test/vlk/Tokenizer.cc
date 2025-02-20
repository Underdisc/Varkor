#include <iostream>
#include <string>

#include "debug/MemLeak.h"
#include "test/Test.h"
#include "vlk/Tokenizer.h"

std::ostream& operator<<(std::ostream& os, Vlk::Token::Type tokenType) {
  switch (tokenType) {
  case Vlk::Token::Type::Key: os << "Key"; break;
  case Vlk::Token::Type::TrueValue: os << "TrueValue"; break;
  case Vlk::Token::Type::OpenBracket: os << "OpenBracket"; break;
  case Vlk::Token::Type::CloseBracket: os << "CloseBracket"; break;
  case Vlk::Token::Type::OpenBrace: os << "OpenBrace"; break;
  case Vlk::Token::Type::CloseBrace: os << "CloseBrace"; break;
  case Vlk::Token::Type::Comma: os << "Comma"; break;
  default: break;
  }
  return os;
}

void PrintTokens(const char* text) {
  // Tokenize the text and print the results.
  VResult<Ds::Vector<Vlk::Token>> result = Vlk::Tokenize(text);
  if (!result.Success()) {
    std::cout << result.mError << '\n';
    return;
  }
  size_t tokenIndex = 0;
  while (result.mValue[(int)tokenIndex].mType != Vlk::Token::Type::Terminator) {
    const Vlk::Token& currentToken = result.mValue[(int)tokenIndex];
    if (currentToken.mType != Vlk::Token::Type::Whitespace) {
      const Vlk::Token& nextToken = result.mValue[(int)tokenIndex + 1];
      size_t tokenLength = nextToken.mText - currentToken.mText;
      std::string tokenText(currentToken.mText, tokenLength);
      std::cout << currentToken.mType << ": " << tokenText << '\n';
    }
    ++tokenIndex;
  }
}

void Key() {
  PrintTokens(
    ":a_key:\n"
    ":a_key_with_1234:\n"
    ":_a_key_with_a_starting_underscore:\n"
    ":A_KEY_WITH_CAPS:\n"
    ":_a_key_with_SOME_CAPS:\n");
}

void TrueValue() {
  PrintTokens(
    "\'1\'\n"
    "\'143\'\n"
    "\'1.0\'\n"
    "\'123.456\'\n"
    "\'ATrueStringValue\'\n"
    "\'WithEscape\\\'Characters\\\\\'\n");
}

void Characters() {
  PrintTokens("[]{},");
}

void InvalidToken() {
  PrintTokens("+");
  PrintTokens("\'an incomplete string");
  PrintTokens(":an incomplete key");
  PrintTokens(":an incomplete key with a newline\n");
  PrintTokens(
    ":this: [\n"
    "  \'is\'\n"
    "  \'testing\'\n"
    "  \'line\n"
    "  \'information\'\n");
}

int main() {
  EnableLeakOutput();
  RunTest(Key);
  RunTest(TrueValue);
  RunTest(Characters);
  RunTest(InvalidToken);
}
