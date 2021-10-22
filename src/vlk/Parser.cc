#include <iostream>
#include <string>

#include "Error.h"
#include "vlk/Parser.h"
#include "vlk/Value.h"

// Valkor Grammar
// Value = PairArray | ValueArray | <Value>
// PairArray = <OpenBrace> Pair* <CloseBrace>
// ValueArray = <OpenBracket> (ValueList | ValueArrayList)? <CloseBracket>
// Pair = <Key> (Value)
// ValueList = Value (<Comma> Value)* <Comma>?
// ValueArrayList = ValueArray (<Comma> ValueArray)* <Comma>?

namespace Vlk {

bool Parser::Accept(Token::Type tokenType)
{
  if (mTokens[mCurrentToken].mType == Token::Type::Terminator)
  {
    return false;
  }
  while (mTokens[mCurrentToken].mType == Token::Type::Whitespace)
  {
    const Token& currentToken = mTokens[mCurrentToken];
    const Token& nextToken = mTokens[mCurrentToken + 1];
    mCurrentLine += CountNewLines(currentToken.mText, nextToken.mText);
    ++mCurrentToken;
  }
  if (mTokens[mCurrentToken].mType == tokenType)
  {
    ++mCurrentToken;
    return true;
  }
  return false;
}

bool Parser::Expect(Token::Type tokenType, const char* error)
{
  return Expect(Accept(tokenType), error);
}

bool Parser::Expect(bool success, const char* error)
{
  if (success)
  {
    return true;
  }
  throw error;
}

const Token& Parser::LastToken()
{
  LogAbortIf(
    mCurrentToken == 0,
    "LastToken can only be used after tokens have been accepted.");
  return mTokens[mCurrentToken - 1];
}

size_t Parser::LastTokenLength()
{
  const Token& lastToken = LastToken();
  return mTokens[mCurrentToken].mText - lastToken.mText;
}

Util::Result Parser::Parse(const char* text, Value* root)
{
  TokenizeResult result = Tokenize(text);
  if (!result.Success())
  {
    return Util::Result(result.mError);
  }
  mCurrentToken = 0;
  mCurrentLine = 1;
  mValueStack.Push(root);
  mTokens = Util::Move(result.mTokens);
  try
  {
    Expect(ParseValue(), "Expected Value.");
  }
  catch (const char* error)
  {
    std::stringstream errorStream;
    errorStream << "[" << mCurrentLine << "] Parse Error: " << error;
    return Util::Result(errorStream.str());
  }
  return Util::Result(true);
}

bool Parser::ParseValue()
{
  if (!Accept(Token::Type::Value))
  {
    return ParsePairArray() || ParseValueArray();
  }
  if (mValueStack.Top()->mType == Value::Type::Invalid)
  {
    mValueStack.Top()->Init(Value::Type::String);
    mValueStack.Push(mValueStack.Top());
  } else
  {
    mValueStack.Top()->HardExpectType(Value::Type::ValueArray);
    mValueStack.Top()->mValueArray.Emplace(Value::Type::String);
    mValueStack.Push(&mValueStack.Top()->mValueArray.Top());
  }
  const Token& valueToken = LastToken();
  mValueStack.Top()->mString.insert(0, valueToken.mText, LastTokenLength());
  mValueStack.Pop();
  return true;
}

bool Parser::ParsePairArray()
{
  if (!Accept(Token::Type::OpenBrace))
  {
    return false;
  }
  mValueStack.Top()->Init(Value::Type::PairArray);
  while (ParsePair())
  {}
  Expect(Token::Type::CloseBrace, "Expected } or Pair.");
  return true;
}

bool Parser::ParseValueArray()
{
  if (!Accept(Token::Type::OpenBracket))
  {
    return false;
  }
  if (mValueStack.Top()->mType == Value::Type::Invalid)
  {
    mValueStack.Top()->Init(Value::Type::ValueArray);
    mValueStack.Push(mValueStack.Top());
  } else
  {
    mValueStack.Top()->HardExpectType(Value::Type::ValueArray);
    mValueStack.Top()->mValueArray.Emplace(Value::Type::ValueArray);
    mValueStack.Push(&mValueStack.Top()->mValueArray.Top());
  }
  ParseValueList() || ParseValueArrayList();
  mValueStack.Pop();
  Expect(Token::Type::CloseBracket, "Expected ].");
  return true;
}

bool Parser::ParsePair()
{
  if (!Accept(Token::Type::Key))
  {
    return false;
  }
  mValueStack.Top()->HardExpectType(Value::Type::PairArray);
  mValueStack.Top()->mPairArray.Emplace();
  mValueStack.Push(&mValueStack.Top()->mPairArray.Top());
  const Token& keyToken = LastToken();
  // We subtract 2 because text is wrapped with two colons.
  size_t keyLength = LastTokenLength() - 2;
  Pair& pair = *(Pair*)mValueStack.Top();
  pair.mKey.insert(0, keyToken.mText + 1, keyLength);
  Expect(ParseValue(), "Expected Value.");
  mValueStack.Pop();
  return true;
}

bool Parser::ParseValueList()
{
  if (!ParseValue())
  {
    return false;
  }
  while (Accept(Token::Type::Comma) && ParseValue())
  {}
  return true;
}

bool Parser::ParseValueArrayList()
{
  if (!ParseValueArray())
  {
    return false;
  }
  while (Accept(Token::Type::Comma) && ParseValueArray())
  {}
  return true;
}

} // namespace Vlk
