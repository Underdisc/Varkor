#include "vlk/Tokenizer.h"

namespace Vlk {

// Qualifier ///////////////////////////////////////////////////////////////////
Qualifier::Range::Range(char start, char end): mStart(start), mEnd(end) {}

bool Qualifier::Range::Contains(char c) const
{
  return mStart <= c && c <= mEnd;
}

void Qualifier::WhitelistChar(char c)
{
  mCharWhitelist.Push(c);
}

void Qualifier::WhitelistRange(char start, char end)
{
  mRangeWhitelist.Emplace(start, end);
}

void Qualifier::Clear()
{
  mCharWhitelist.Clear();
  mRangeWhitelist.Clear();
}

bool Qualifier::Qualify(char c) const
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

// Tokenizer ///////////////////////////////////////////////////////////////////
Tokenizer::State::State(Token::Type tokenType):
  mDefault(smInvalidStateIndex), mTokenType(tokenType)
{}

void Tokenizer::State::AddEdge(StateIndex to, const Qualifier& q)
{
  Edge edge = {to, q};
  mEdges.Push(edge);
}

Tokenizer::StateIndex Tokenizer::State::NextState(char c)
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

Tokenizer::Tokenizer()
{
  // Add all of the states and edges needed for the Tokenizer's DFA.
  mRoot = AddState(Token::Type::Invalid);
  {
    // Key
    StateIndex accept = AddState(Token::Type::Key);
    Qualifier q;
    q.WhitelistRange('a', 'z');
    q.WhitelistRange('A', 'Z');
    q.WhitelistChar('_');
    AddEdge(mRoot, accept, q);
    q.WhitelistRange('0', '9');
    AddEdge(accept, accept, q);
  }
  {
    // Value
    StateIndex accept0 = AddState(Token::Type::Value);
    StateIndex accept1 = AddState(Token::Type::Value);
    StateIndex noAccept0 = AddState(Token::Type::Invalid);
    StateIndex noAccept1 = AddState(Token::Type::Invalid);
    Qualifier q;
    q.WhitelistRange('0', '9');
    AddEdge(mRoot, accept0, q);
    AddEdge(accept0, accept0, q);

    q.Clear();
    q.WhitelistChar('.');
    AddEdge(accept0, noAccept1, q);

    q.Clear();
    q.WhitelistRange('0', '9');
    AddEdge(noAccept1, accept1, q);
    AddEdge(accept1, accept1, q);

    q.Clear();
    q.WhitelistChar('"');
    AddEdge(mRoot, noAccept0, q);
    AddDefault(noAccept0, noAccept0);
    AddEdge(noAccept0, accept0, q);
  }
  {
    // Whitespace
    StateIndex accept = AddState(Token::Type::Whitespace);
    Qualifier q;
    q.WhitelistChar(' ');
    q.WhitelistChar('\n');
    AddEdge(mRoot, accept, q);
    AddEdge(accept, accept, q);
  }
  AddLoneState(Token::Type::Colon, ':');
  AddLoneState(Token::Type::OpenBracket, '[');
  AddLoneState(Token::Type::CloseBracket, ']');
  AddLoneState(Token::Type::OpenBrace, '{');
  AddLoneState(Token::Type::CloseBrace, '}');
}

Tokenizer::StateIndex Tokenizer::AddState(Token::Type tokenType)
{
  mStates.Emplace(tokenType);
  return mStates.Size() - 1;
}

void Tokenizer::AddLoneState(Token::Type tokenType, char c)
{
  StateIndex accept = AddState(tokenType);
  Qualifier q;
  q.WhitelistChar(c);
  AddEdge(mRoot, accept, q);
}

void Tokenizer::AddEdge(StateIndex from, StateIndex to, const Qualifier& q)
{
  State& fromState = mStates[from];
  fromState.AddEdge(to, q);
}

void Tokenizer::AddDefault(StateIndex from, StateIndex to)
{
  State& fromState = mStates[from];
  fromState.mDefault = to;
}

Tokenizer::Result Tokenizer::Tokenize(const char* text)
{
  // Tokenize the text and remove all whitespace while doing so.
  Result result;
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

Tokenizer::Token Tokenizer::ReadNextToken(const char* text)
{
  int lastLength = -1;
  StateIndex last;
  StateIndex current = mRoot;
  while (current != smInvalidStateIndex)
  {
    ++lastLength;
    last = current;
    current = mStates[current].NextState(text[lastLength]);
  }

  // When first character does not qualify for any edges leading from the root,
  // it is an invalid Token consisting of one character.
  if (last == mRoot)
  {
    Token token = {text, 1, Token::Type::Invalid};
    return token;
  }
  Token token = {text, lastLength, mStates[last].mTokenType};
  return token;
}

} // namespace Vlk
