#include <iostream>
#include <string>

#include "vlk/Pair.h"
#include "vlk/Parser.h"

// Valkor Grammar
// Root = Pair | PairArray
// PairArray = <OpenBrace> Pair* <CloseBrace>
// Pair = <Key> (PairArray | ValueArray | ValueSingle)
// ValueArray = <OpenBracket> (ValueSingleList | ValueArrayList)? <CloseBracket>
// ValueSingleList = ValueSingle (<Comma> ValueSingle)* <Comma>?
// ValueArrayList = ValueArray (<Comma> ValueArray)* <Comma>?
// ValueSingle = <Value>

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

Util::Result Parser::Parse(const char* text, Pair* rootPair)
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
    if (!ParseRoot(rootPair))
    {
      return Util::Result("Expected PairArray or Pair.");
    }
    return Util::Result();
  }
  catch (const char* error)
  {
    errorText = error;
  }
  return Util::Result(errorText);
}

bool Parser::ParseRoot(Pair* rootPair)
{
  return ParsePair(rootPair) || ParsePairArray(rootPair);
}

bool Parser::ParsePairArray(Pair* rootPair)
{
  if (!Accept(Token::Type::OpenBrace))
  {
    return false;
  }
  if (rootPair != nullptr)
  {
    mPairStack.Push(rootPair);
  }
  mPairStack.Top()->mValue.Init(Pair::Value::Type::PairArray);
  while (ParsePair())
  {}
  Expect(Token::Type::CloseBrace, "Expected } or Pair.");
  return true;
}

bool Parser::ParsePair(Pair* rootPair)
{
  if (!Accept(Token::Type::Key))
  {
    return false;
  }
  if (rootPair != nullptr)
  {
    mPairStack.Push(rootPair);
  } else
  {
    Pair& upPair = *mPairStack.Top();
    upPair.mValue.mPairArray.Emplace();
    mPairStack.Push(&upPair.mValue.mPairArray.Top());
  }
  const Token& keyToken = LastToken();
  mPairStack.Top()->mKey.insert(0, keyToken.mText + 1, keyToken.mLength - 2);
  Expect(
    ParsePairArray() || ParseValueArray() || ParseValueSingle(),
    "Expected ValueSingle, PairArray, or ValueArray.");
  mPairStack.Pop();
  return true;
}

bool Parser::ParseValueArray()
{
  if (!Accept(Token::Type::OpenBracket))
  {
    return false;
  }
  Pair::Value* newValue = nullptr;
  if (mValueStack.Size() == 0)
  {
    Pair& pair = *mPairStack.Top();
    pair.mValue.Init(Pair::Value::Type::ValueArray);
    newValue = &pair.mValue;
  } else
  {
    Pair::Value& upValue = *mValueStack.Top();
    upValue.mValueArray.Emplace(Pair::Value::Type::ValueArray);
    newValue = &upValue.mValueArray.Top();
  }
  mValueStack.Push(newValue);
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
  Pair::Value* newValue = nullptr;
  if (mValueStack.Size() == 0)
  {
    Pair& pair = *mPairStack.Top();
    pair.mValue.Init(Pair::Value::Type::ValueSingle);
    newValue = &pair.mValue;
  } else
  {
    Pair::Value& upValue = *mValueStack.Top();
    upValue.mValueArray.Emplace(Pair::Value::Type::ValueSingle);
    newValue = &upValue.mValueArray.Top();
  }
  newValue->mValueSingle.insert(0, valueToken.mText, valueToken.mLength);
  return true;
}

} // namespace Vlk
