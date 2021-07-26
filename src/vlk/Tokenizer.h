#ifndef vlk_Tokenizer_h
#define vlk_Tokenizer_h

#include "ds/Vector.h"
#include "util/Utility.h"

namespace Vlk {

struct Token
{
  enum class Type
  {
    Invalid,
    Key,
    Value,
    Whitespace,
    OpenBracket,
    CloseBracket,
    OpenBrace,
    CloseBrace,
    Comma,
  };
  const char* mText;
  int mLength;
  Type mType;
};

struct TokenizeResult: Util::Result
{
  Ds::Vector<Token> mTokens;
};
TokenizeResult Tokenize(const char* text);

} // namespace Vlk

#endif
