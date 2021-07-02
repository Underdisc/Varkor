#include "vlk/Tokenizer.h"

namespace Vlk {

// Qualifier ///////////////////////////////////////////////////////////////////
struct Qualifier
{
private:
  struct Range
  {
    Range(char start, char end): mStart(start), mEnd(end) {}
    bool Contains(char c) const
    {
      return mStart <= c && c <= mEnd;
    }
    char mStart;
    char mEnd;
  };
  Ds::Vector<char> mCharWhitelist;
  Ds::Vector<Range> mRangeWhitelist;

public:
  void WhitelistChar(char c)
  {
    mCharWhitelist.Push(c);
  }

  void WhitelistRange(char start, char end)
  {
    mRangeWhitelist.Emplace(start, end);
  }

  void Clear()
  {
    mCharWhitelist.Clear();
    mRangeWhitelist.Clear();
  }

  bool Qualify(char c) const
  {
    for (char whitelistChar : mCharWhitelist)
    {
      if (c == whitelistChar)
      {
        return true;
      }
    }
    for (const Range& range : mRangeWhitelist)
    {
      if (range.Contains(c))
      {
        return true;
      }
    }
    return false;
  }
};

// State ///////////////////////////////////////////////////////////////////////
// A StateIndex is used to reference a State instead of a State pointer because
// the States are stored in a vector. State pointers become invalid when the
// vector grows.
typedef int StateIndex;
static constexpr StateIndex smInvalidStateIndex = -1;

struct State
{
  struct Edge
  {
    StateIndex mTo;
    Qualifier mQ;
  };
  Ds::Vector<Edge> mEdges;
  StateIndex mDefault;
  Token::Type mTokenType;

  State(Token::Type tokenType):
    mDefault(smInvalidStateIndex), mTokenType(tokenType)
  {}

  void AddEdge(StateIndex to, const Qualifier& q)
  {
    Edge edge = {to, q};
    mEdges.Push(edge);
  }

  StateIndex NextState(char c)
  {
    for (const Edge& edge : mEdges)
    {
      if (edge.mQ.Qualify(c))
      {
        return edge.mTo;
      }
    }
    if (c == '\0')
    {
      return smInvalidStateIndex;
    }
    return mDefault;
  }
};

// Tokenizer ///////////////////////////////////////////////////////////////////
Ds::Vector<State> nStates;
StateIndex nRoot;

StateIndex AddState(Token::Type tokenType)
{
  nStates.Emplace(tokenType);
  return nStates.Size() - 1;
}

void AddEdge(StateIndex from, StateIndex to, const Qualifier& q)
{
  nStates[from].AddEdge(to, q);
}

void AddDefault(StateIndex from, StateIndex to)
{
  nStates[from].mDefault = to;
}

void AddLoneState(Token::Type tokenType, char c)
{
  StateIndex accept = AddState(tokenType);
  Qualifier q;
  q.WhitelistChar(c);
  AddEdge(nRoot, accept, q);
}

void InitTokenizer()
{
  // Add all of the states and edges needed for the Tokenizer's DFA.
  nRoot = AddState(Token::Type::Invalid);
  // Key
  {
    StateIndex accept = AddState(Token::Type::Key);
    StateIndex noAccept = AddState(Token::Type::Invalid);
    Qualifier q;
    q.WhitelistChar(':');
    AddEdge(nRoot, noAccept, q);
    AddEdge(noAccept, accept, q);
    AddDefault(noAccept, noAccept);
  }
  // Value
  {
    StateIndex accept0 = AddState(Token::Type::Value);
    StateIndex accept1 = AddState(Token::Type::Value);
    StateIndex noAccept0 = AddState(Token::Type::Invalid);
    StateIndex noAccept1 = AddState(Token::Type::Invalid);
    StateIndex noAccept2 = AddState(Token::Type::Invalid);
    Qualifier q;
    q.WhitelistChar('-');
    AddEdge(nRoot, noAccept2, q);

    q.Clear();
    q.WhitelistRange('0', '9');
    AddEdge(nRoot, accept0, q);
    AddEdge(accept0, accept0, q);
    AddEdge(noAccept2, accept0, q);

    q.Clear();
    q.WhitelistChar('.');
    AddEdge(accept0, noAccept1, q);

    q.Clear();
    q.WhitelistRange('0', '9');
    AddEdge(noAccept1, accept1, q);
    AddEdge(accept1, accept1, q);

    q.Clear();
    q.WhitelistChar('"');
    AddEdge(nRoot, noAccept0, q);
    AddDefault(noAccept0, noAccept0);
    AddEdge(noAccept0, accept0, q);
  }
  // Whitespace
  {
    StateIndex accept = AddState(Token::Type::Whitespace);

    Qualifier q;
    q.WhitelistChar(' ');
    q.WhitelistChar('\n');
    AddEdge(nRoot, accept, q);
    AddEdge(accept, accept, q);
  }
  // Lone Characters
  AddLoneState(Token::Type::OpenBracket, '[');
  AddLoneState(Token::Type::CloseBracket, ']');
  AddLoneState(Token::Type::OpenBrace, '{');
  AddLoneState(Token::Type::CloseBrace, '}');
  AddLoneState(Token::Type::Comma, ',');
}

Token ReadNextToken(const char* text)
{
  int lastLength = -1;
  StateIndex last;
  StateIndex current = nRoot;
  while (current != smInvalidStateIndex)
  {
    ++lastLength;
    last = current;
    current = nStates[current].NextState(text[lastLength]);
  }

  // When first character does not qualify for any edges leading from the root,
  // it is an invalid Token consisting of one character.
  if (last == nRoot)
  {
    Token token = {text, 1, Token::Type::Invalid};
    return token;
  }
  Token token = {text, lastLength, nStates[last].mTokenType};
  return token;
}

TokenizeResult Tokenize(const char* text)
{
  // Initialize the Tokenizer if it hasn't been.
  if (nStates.Empty())
  {
    InitTokenizer();
  }
  // Tokenize the text and remove all whitespace Tokens.
  TokenizeResult result;
  while (*text != '\0')
  {
    Token token = ReadNextToken(text);
    switch (token.mType)
    {
    case Token::Type::Invalid:
      result.mError += "Invalid token: \"";
      result.mError.append(token.mText, token.mLength);
      result.mError += "\"";
      return result;
    case Token::Type::Whitespace: text += token.mLength; break;
    default: result.mTokens.Push(token); text = text + token.mLength;
    }
  }
  return result;
}

} // namespace Vlk
