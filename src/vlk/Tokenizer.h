#ifndef vlk_Tokenizer_h
#define vlk_Tokenizer_h

#include "Result.h"
#include "ds/Vector.h"

namespace Vlk {

struct Token
{
  enum class Type
  {
    Invalid,
    Whitespace,
    Key,
    TrueValue,
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

VResult<Ds::Vector<Token>> Tokenize(const char* text);
size_t CountNewLines(const char* start, const char* end);

} // namespace Vlk

#endif
