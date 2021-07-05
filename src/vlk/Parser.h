#ifndef vlk_Parser_h
#define vlk_Parser_h

#include "ds/Vector.h"
#include "util/Utility.h"
#include "vlk/Tokenizer.h"

namespace Vlk {

struct Pair;
struct Pair::Value;

struct Parser
{
public:
  Util::Result Parse(const char* text, Pair* pair);

private:
  int mCurrentToken;
  Ds::Vector<Pair*> mPairStack;
  Ds::Vector<Pair::Value*> mValueStack;
  Ds::Vector<Token> mTokens;

  bool Accept(Token::Type tokenType);
  bool Expect(Token::Type tokenType, const char* error);
  bool Expect(bool success, const char* error);
  const Token& LastToken();

  bool ParseRoot(Pair* rootPair);
  bool ParsePairArray(Pair* rootPair = nullptr);
  bool ParsePair(Pair* rootPair = nullptr);
  bool ParseValueArray();
  bool ParseValueSingleList();
  bool ParseValueArrayList();
  bool ParseValueSingle();
};

} // namespace Vlk

#endif
