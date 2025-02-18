#ifndef vlk_Parser_h
#define vlk_Parser_h

#include "Result.h"
#include "ds/Vector.h"
#include "vlk/Tokenizer.h"
#include "vlk/Value.h"

namespace Vlk {

struct Parser {
public:
  Result Parse(const char* text, Value* root);

private:
  size_t mCurrentToken;
  size_t mCurrentLine;
  Ds::Vector<Value*> mValueStack;
  Ds::Vector<Token> mTokens;

  bool Accept(Token::Type tokenType);
  bool Expect(Token::Type tokenType, const char* error);
  bool Expect(bool success, const char* error);
  const Token& LastToken();
  size_t LastTokenLength();

  bool ParseValue();
  bool ParsePairArray();
  bool ParseValueArray();
  bool ParsePair();
  bool ParseValueList();
  bool ParseValueArrayList();
};

} // namespace Vlk

#endif
