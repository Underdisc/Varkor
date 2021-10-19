#include <sstream>

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
static constexpr StateIndex nInvalidTerminal = -1;

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
    mDefault(nInvalidTerminal), mTokenType(tokenType)
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
      return nInvalidTerminal;
    }
    return mDefault;
  }
};

// Tokenizer ///////////////////////////////////////////////////////////////////
Ds::Vector<State> nStates;
StateIndex nRoot;

StateIndex AddStates(Token::Type tokenType, size_t amount)
{
  for (size_t i = 0; i < amount; ++i)
  {
    nStates.Emplace(tokenType);
  }
  return (StateIndex)(nStates.Size() - amount);
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
  StateIndex valid = AddStates(tokenType, 1);
  Qualifier q;
  q.WhitelistChar(c);
  AddEdge(nRoot, valid, q);
}

void InitTokenizer()
{
  // Add all of the states and edges needed for the Tokenizer's DFA.
  nRoot = AddStates(Token::Type::Invalid, 1);

  // Key
  {
    StateIndex valid = AddStates(Token::Type::Key, 1);
    StateIndex invalid = AddStates(Token::Type::Invalid, 1);

    Qualifier q;
    q.WhitelistChar(':');
    AddEdge(nRoot, invalid, q);
    AddEdge(invalid, valid, q);
    AddDefault(invalid, invalid);

    q.Clear();
    q.WhitelistChar('\n');
    AddEdge(invalid, nInvalidTerminal, q);
  }
  // Value
  {
    StateIndex valid = AddStates(Token::Type::Value, 2);
    StateIndex invalid = AddStates(Token::Type::Invalid, 3);

    Qualifier q;
    q.WhitelistChar('-');
    AddEdge(nRoot, invalid + 2, q);

    q.Clear();
    q.WhitelistRange('0', '9');
    AddEdge(nRoot, valid, q);
    AddEdge(valid, valid, q);
    AddEdge(invalid + 2, valid, q);

    q.Clear();
    q.WhitelistChar('.');
    AddEdge(valid, invalid + 1, q);

    q.Clear();
    q.WhitelistRange('0', '9');
    AddEdge(invalid + 1, valid + 1, q);
    AddEdge(valid + 1, valid + 1, q);

    q.Clear();
    q.WhitelistChar('"');
    AddEdge(nRoot, invalid, q);
    AddDefault(invalid, invalid);
    AddEdge(invalid, valid, q);

    q.Clear();
    q.WhitelistChar('\n');
    AddEdge(invalid, nInvalidTerminal, q);
  }
  // Whitespace
  {
    StateIndex valid = AddStates(Token::Type::Whitespace, 1);
    Qualifier q;
    q.WhitelistChar(' ');
    q.WhitelistChar('\t');
    q.WhitelistChar('\n');
    AddEdge(nRoot, valid, q);
    AddEdge(valid, valid, q);
  }
  // Lone Characters
  AddLoneState(Token::Type::OpenBracket, '[');
  AddLoneState(Token::Type::CloseBracket, ']');
  AddLoneState(Token::Type::OpenBrace, '{');
  AddLoneState(Token::Type::CloseBrace, '}');
  AddLoneState(Token::Type::Comma, ',');
}

size_t CountNewLines(const char* start, const char* end)
{
  size_t newLineCount = 0;
  const char* currentChar = start;
  while (currentChar < end)
  {
    if (*currentChar == '\n')
    {
      ++newLineCount;
    }
    ++currentChar;
  }
  return newLineCount;
}

Token ReadNextToken(const char** text)
{
  const char* start = *text;
  StateIndex currentState = nRoot;
  StateIndex nextState = nStates[nRoot].NextState(**text);
  while (nextState != nInvalidTerminal)
  {
    ++(*text);
    currentState = nextState;
    nextState = nStates[currentState].NextState(**text);
  }

  // When first character does not qualify for any edges leading from the root,
  // it is an invalid Token consisting of one character.
  if (currentState == nRoot)
  {
    ++(*text);
    Token token = {start, Token::Type::Invalid};
    return token;
  }
  Token token = {start, nStates[currentState].mTokenType};
  return token;
}

TokenizeResult Tokenize(const char* text)
{
  // Initialize the Tokenizer if it hasn't been.
  if (nStates.Empty())
  {
    InitTokenizer();
  }

  // Tokenize the text.
  size_t lineNumber = 1;
  std::stringstream error;
  TokenizeResult result;
  while (*text != '\0')
  {
    Token token = ReadNextToken(&text);
    switch (token.mType)
    {
    case Token::Type::Invalid:
      error << "[" << lineNumber << "] Invalid token: "
            << std::string(token.mText, text - token.mText);
      result.mError = error.str();
      return result;
    case Token::Type::Whitespace:
      lineNumber += CountNewLines(token.mText, text);
    default: result.mTokens.Push(token);
    }
  }
  result.mTokens.Push({text, Token::Type::Terminator});
  return result;
}

} // namespace Vlk
