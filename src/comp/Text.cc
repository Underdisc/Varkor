#include <istream>
#include <ostream>

#include "comp/Text.h"
#include "gfx/Font.h"
#include "math/Vector.h"
#include "util/Utility.h"

namespace Comp {

void Text::VInit(const World::Object& owner)
{
  mFontId = AssLib::nDefaultAssetId;
  mShaderId = AssLib::nDefaultTextShaderId;
  mText = "";
  mAlign = Alignment::Left;
  mWidth = 10.0f;
  mColor = {1.0f, 1.0f, 1.0f};
  mFillAmount = 1.0f;
}

void Text::VSerialize(Vlk::Value& textVal)
{
  textVal("FontId") = mFontId;
  textVal("ShaderId") = mShaderId;
  textVal("Text") = mText;
  textVal("Alignment") = mAlign;
  textVal("Width") = mWidth;
  Vlk::Value& colorVal = textVal("Color")[{3}];
  for (int i = 0; i < 3; ++i)
  {
    colorVal[i] = mColor[i];
  }
}

void Text::VDeserialize(const Vlk::Explorer& textEx)
{
  mFontId = textEx("FontId").As<AssetId>(AssLib::nDefaultAssetId);
  mShaderId = textEx("ShaderId").As<AssetId>(AssLib::nDefaultAssetId);
  mText = textEx("Text").As<std::string>("");
  mAlign = textEx("Alignment").As<Alignment>(Alignment::Left);
  mWidth = textEx("Width").As<float>(10.0f);
  const Vlk::Explorer& colorEx = textEx("Color");
  for (int i = 0; i < 3; ++i)
  {
    mColor[i] = colorEx[i].As<float>(1.0f);
  }
  mFillAmount = 1.0f;
}

Ds::Vector<Text::Line> Text::GetLines() const
{
  Ds::Vector<Line> lines;
  Gfx::Font* font = AssLib::TryGetLive<Gfx::Font>(mFontId);
  if (font == nullptr)
  {
    return lines;
  }

  auto isWhitespace = [](char codepoint) -> bool
  {
    return (codepoint == ' ' || codepoint == '\t' || codepoint == '\n');
  };
  constexpr int invalidLoc = -1;
  int prevWordEnd = invalidLoc;
  int wordStart = invalidLoc;
  int lineWordCount = 0;
  int lineStart = 0;
  int lineEnd = invalidLoc;
  float currentWidth = 0.0f;
  for (int i = 0; i < mText.size(); ++i)
  {
    // Find if we go into or out of a word.
    char codepoint = mText[i];
    if (!isWhitespace(codepoint) && wordStart == invalidLoc)
    {
      wordStart = i;
    }
    if (isWhitespace(codepoint) && wordStart != invalidLoc)
    {
      prevWordEnd = i;
      ++lineWordCount;
      wordStart = invalidLoc;
    }

    // End the line if a newline is encountered or if the line's width has
    // exceeded the max and it has at least one word.
    if (codepoint == '\n')
    {
      if (lineWordCount >= 1)
      {
        lineEnd = prevWordEnd;
      } else
      {
        lineEnd = lineStart;
      }
    }
    const Gfx::Font::GlyphMetrics& glyphM = font->GetGlyphMetrics(codepoint);
    currentWidth += glyphM.mAdvance;
    if (currentWidth > mWidth && lineWordCount >= 1)
    {
      lineEnd = prevWordEnd;
    }

    // If the end has been chosen, add the line and prepare for the next.
    if (lineEnd != invalidLoc)
    {
      lines.Push({lineStart, lineEnd});
      if (lineEnd == lineStart)
      {
        lineStart = i + 1;
      } else
      {
        lineStart = lineEnd + 1;
        wordStart = invalidLoc;
        lineWordCount = 0;
      }
      lineEnd = invalidLoc;
      currentWidth = 0.0f;
    }
  }
  lines.Push({lineStart, (int)mText.size()});

  // Find the length of each line.
  for (Line& line : lines)
  {
    line.mWidth = 0.0f;
    for (size_t i = line.mStart; i < line.mEnd; ++i)
    {
      const Gfx::Font::GlyphMetrics& glyphM = font->GetGlyphMetrics(mText[i]);
      line.mWidth += glyphM.mAdvance;
    }
  }
  return lines;
}

Ds::Vector<Text::DrawInfo> Text::GetAllDrawInfo() const
{
  Ds::Vector<DrawInfo> drawInfo;
  Gfx::Font* font = AssLib::TryGetLive<Gfx::Font>(mFontId);
  if (font == nullptr)
  {
    return drawInfo;
  }

  Vec2 baselineOffset = {0.0f, 0.0f};
  float halfWidth = mWidth / 2.0f;
  size_t currentCharacter = 0;
  Ds::Vector<Line> lines = GetLines();
  for (const Line& line : lines)
  {
    switch (mAlign)
    {
    case Alignment::Left: baselineOffset[0] = -halfWidth; break;
    case Alignment::Center: baselineOffset[0] = -line.mWidth / 2.0f; break;
    case Alignment::Right: baselineOffset[0] = halfWidth - line.mWidth;
    }
    for (size_t i = line.mStart; i < line.mEnd; ++i)
    {
      // Find the scale that maintains the character's aspect ratio and the
      // translation offset from the text's relative origin.
      const Gfx::Font::GlyphMetrics& glyphM = font->GetGlyphMetrics(mText[i]);
      Mat4 scale, translate;
      Vec2 diagonal = glyphM.mEndOffset - glyphM.mStartOffset;
      Math::Scale(&scale, {diagonal[0], diagonal[1], 1.0f});
      Vec2 centerOffset = (glyphM.mStartOffset + glyphM.mEndOffset) / 2.0f;
      Vec2 fullOffset = baselineOffset + centerOffset;
      Math::Translate(&translate, {fullOffset[0], fullOffset[1], 0.0f});
      GLuint textureId = font->GetTextureId(mText[i]);
      drawInfo.Push({textureId, translate * scale});
      baselineOffset[0] += glyphM.mAdvance;
    }
    baselineOffset[1] -= font->NewlineOffset();
  }
  return drawInfo;
}

std::ostream& operator<<(std::ostream& os, const Text::Alignment& align)
{
  switch (align)
  {
  case Text::Alignment::Left: os << "Left"; break;
  case Text::Alignment::Center: os << "Center"; break;
  case Text::Alignment::Right: os << "Right"; break;
  }
  return os;
}

std::istream& operator>>(std::istream& is, Text::Alignment& align)
{
  std::string alignString;
  is >> alignString;
  align = Text::Alignment::Left;
  if (alignString == "Center")
  {
    align = Text::Alignment::Center;
  } else if (alignString == "Right")
  {
    align = Text::Alignment::Right;
  }
  return is;
}

} // namespace Comp
