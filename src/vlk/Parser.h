#ifndef vlk_Parser_h
#define vlk_Parser_h

#include "ds/Vector.h"
#include "util/Utility.h"
#include "vlk/Pair.h"
#include "vlk/Tokenizer.h"

namespace Vlk {

struct Parser
{
public:
  Util::Result Parse(const char* text, Value* root);

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

  bool ParseRoot(Value* root);
  bool ParsePairArray(Value* root = nullptr);
  bool ParsePair(Value* root = nullptr);
  bool ParseValueArray();
  bool ParseValueList();
  bool ParseValueArrayList();
  bool ParseValue();
};

} // namespace Vlk

#endif
