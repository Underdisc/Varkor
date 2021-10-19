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
    Whitespace,
    Key,
    Value,
    OpenBracket,
    CloseBracket,
    OpenBrace,
    CloseBrace,
    Comma,
    Terminator,
  };
  const char* mText;
  Type mType;
};

struct TokenizeResult: Util::Result
{
  Ds::Vector<Token> mTokens;
};
TokenizeResult Tokenize(const char* text);

size_t CountNewLines(const char* start, const char* end);

} // namespace Vlk

#endif
