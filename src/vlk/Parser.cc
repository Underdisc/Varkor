#include <iostream>
#include <string>

#include "Error.h"
#include "vlk/Pair.h"
#include "vlk/Parser.h"

// Valkor Grammar
// Root = Pair | PairArray
// Pair = <Key> (ValueSingle | PairArray | ValueArray)
// ValueSingle = <Value>
// PairArray = <OpenBrace> Pair* <CloseBrace>
// ValueArray = <OpenBracket> (ValueSingleList | ValueArrayList)? <CloseBracket>
// ValueSingleList = ValueSingle (<Comma> ValueSingle)* <Comma>?
// ValueArrayList = ValueArray (<Comma> ValueArray)* <Comma>?

// There is a distinction between the ValueSingle grammar rule and the <Value>
// token because a ValueSingle function is part of the Parser. This makes it
// easier to deserialize a Value token into a Pair::Value.

namespace Vlk {

bool Parser::Accept(Token::Type tokenType)
{
  if (mCurrentToken >= mTokens.Size())
  {
    return false;
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
    mCurrentToken <= 0,
    "LastToken can only be used after tokens have been accepted.");
  return mTokens[mCurrentToken - 1];
}

Util::Result Parser::Parse(const char* text, Value* root)
{
  mCurrentToken = 0;
  TokenizeResult result = Tokenize(text);
  if (!result.Success())
  {
    return Util::Result(result.mError);
  }
  mTokens = Util::Move(result.mTokens);
  const char* errorText = "";
  try
  {
    if (!ParseRoot(root))
    {
      return Util::Result("Expected PairArray or Pair.");
    }
    return Util::Result(true);
  }
  catch (const char* error)
  {
    errorText = error;
  }
  return Util::Result(errorText);
}

bool Parser::ParseRoot(Value* root)
{
  return ParsePair(root) || ParsePairArray(root);
}

bool Parser::ParsePairArray(Value* root)
{
  if (!Accept(Token::Type::OpenBrace))
  {
    return false;
  }
  if (root != nullptr)
  {
    mValueStack.Push(root);
  }
  mValueStack.Top()->Init(Value::Type::PairArray);
  while (ParsePair())
  {}
  Expect(Token::Type::CloseBrace, "Expected } or Pair.");
  return true;
}

bool Parser::ParsePair(Value* root)
{
  if (!Accept(Token::Type::Key))
  {
    return false;
  }
  if (root != nullptr)
  {
    mValueStack.Push(root);
  } else
  {
    mValueStack.Top()->mPairArray.Emplace();
    mValueStack.Push(&mValueStack.Top()->mPairArray.Top());
  }
  const Token& keyToken = LastToken();
  Pair& pair = *(Pair*)mValueStack.Top();
  pair.mKey.insert(0, keyToken.mText + 1, keyToken.mLength - 2);
  Expect(
    ParsePairArray() || ParseValueArray() || ParseValueSingle(),
    "Expected ValueSingle, PairArray, or ValueArray.");
  mValueStack.Pop();
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
  ParseValueSingleList() || ParseValueArrayList();
  mValueStack.Pop();
  Expect(Token::Type::CloseBracket, "Expected ].");
  return true;
}

bool Parser::ParseValueSingleList()
{
  if (!ParseValueSingle())
  {
    return false;
  }
  while (Accept(Token::Type::Comma) && ParseValueSingle())
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

bool Parser::ParseValueSingle()
{
  if (!Accept(Token::Type::Value))
  {
    return false;
  }
  const Token& valueToken = LastToken();
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
  mValueStack.Top()->mString.insert(0, valueToken.mText, valueToken.mLength);
  mValueStack.Pop();
  return true;
}

} // namespace Vlk
