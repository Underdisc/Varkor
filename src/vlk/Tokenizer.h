#ifndef vlk_Tokenizer_h
#define vlk_Tokenizer_h

#include <string>

#include "ds/Vector.h"

namespace Vlk {

struct Qualifier
{
private:
  struct Range
  {
    Range(char start, char end);
    bool Contains(char c) const;
    char mStart;
    char mEnd;
  };
  Ds::Vector<char> mCharWhitelist;
  Ds::Vector<Range> mRangeWhitelist;

public:
  void WhitelistChar(char c);
  void WhitelistRange(char start, char end);
  void Clear();
  bool Qualify(char c) const;
};

struct Tokenizer
{
public:
  struct Token
  {
    enum class Type
    {
      Invalid,
      Key,
      Value,
      Whitespace,
      Colon,
      OpenBracket,
      CloseBracket,
      OpenBrace,
      CloseBrace,
    };
    const char* mText;
    int mLength;
    Type mType;
  };
  struct Result
  {
    Ds::Vector<Token> mTokens;
    std::string mError;
  };

  Tokenizer();
  Result Tokenize(const char* text);

private:
  // A StateIndex is used to reference a State instead of a State pointer
  // because the States are stored in a vector. State pointers become invalid
  // when the vector grows.
  typedef int StateIndex;
  static constexpr StateIndex smInvalidStateIndex = -1;
  struct Edge
  {
    StateIndex mTo;
    Qualifier mQ;
  };
  struct State
  {
    State(Token::Type tokenType);
    void AddEdge(StateIndex to, const Qualifier& q);
    StateIndex NextState(char c);

    Ds::Vector<Edge> mEdges;
    StateIndex mDefault;
    Token::Type mTokenType;
  };

  Ds::Vector<State> mStates;
  StateIndex mRoot;

  StateIndex AddState(Token::Type tokenType);
  void AddLoneState(Token::Type tokenType, char c);
  void AddEdge(StateIndex from, StateIndex to, const Qualifier& q);
  void AddDefault(StateIndex from, StateIndex to);
  Token ReadNextToken(const char* text);
};

typedef Tokenizer::Token Token;

} // namespace Vlk

#endif
