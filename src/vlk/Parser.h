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
  int mCurrentToken;
  Ds::Vector<Value*> mValueStack;
  Ds::Vector<Token> mTokens;

  bool Accept(Token::Type tokenType);
  bool Expect(Token::Type tokenType, const char* error);
  bool Expect(bool success, const char* error);
  const Token& LastToken();

  bool ParseRoot(Value* root);
  bool ParsePairArray(Value* root = nullptr);
  bool ParsePair(Value* root = nullptr);
  bool ParseValueArray();
  bool ParseValueSingleList();
  bool ParseValueArrayList();
  bool ParseValueSingle();
};

} // namespace Vlk

#endif
