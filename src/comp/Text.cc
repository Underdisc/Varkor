#include <imgui/imgui.h>
#include <istream>
#include <ostream>

#include "comp/Text.h"
#include "comp/Transform.h"
#include "editor/Utility.h"
#include "gfx/Renderer.h"
#include "gfx/UniformVector.h"
#include "math/Vector.h"
#include "rsl/Library.h"
#include "util/Utility.h"

namespace Comp {

const ResId Text::smDefaultFontId(Text::smDefaultAssetName, "Default");
const ResId Text::smDefaultMaterialId(Text::smDefaultAssetName, "Default");

void Text::VStaticInit()
{
  Rsl::RequireAsset(smDefaultAssetName);
}

void Text::VInit(const World::Object& owner)
{
  mFontId = smDefaultFontId;
  mMaterialId = smDefaultMaterialId;

  mText = "";
  mAlign = Alignment::Left;
  mWidth = 10.0f;
  mVisible = true;
  mFillAmount = 1.0f;
  mColor = {1.0f, 1.0f, 1.0f, 1.0f};
}

void Text::VSerialize(Vlk::Value& textVal)
{
  textVal("FontId") = mFontId;
  textVal("Material") = mMaterialId;
  textVal("Text") = mText;
  textVal("Alignment") = mAlign;
  textVal("Width") = mWidth;
  textVal("Visible") = mVisible;
  textVal("FillAmount") = mFillAmount;
  textVal("Color") = mColor;
}

void Text::VDeserialize(const Vlk::Explorer& textEx)
{
  mFontId = textEx("FontId").As<ResId>(smDefaultFontId);
  mMaterialId = textEx("MaterialId").As<ResId>(smDefaultMaterialId);
  mText = textEx("Text").As<std::string>("");
  mAlign = textEx("Alignment").As<Alignment>(Alignment::Left);
  mVisible = textEx("Visible").As<bool>(true);
  mWidth = textEx("Width").As<float>(10.0f);
  mFillAmount = textEx("FillAmount").As<float>(1.0f);
  mColor = textEx("Color").As<Vec4>({1.0f, 1.0f, 1.0f, 1.0f});
}

void Text::VRenderable(const World::Object& owner)
{
  Gfx::Font* font = Rsl::TryGetRes<Gfx::Font>(mFontId);
  if (font == nullptr) {
    return;
  }

  Vec2 baselineOffset = {0.0f, 0.0f};
  float halfWidth = mWidth / 2.0f;
  size_t currentCharacter = 0;
  auto& transformComp = owner.Get<Comp::Transform>();
  const Mat4& ownerTransformation = transformComp.GetWorldMatrix(owner);
  Ds::Vector<Line> lines = GetLines(*font);
  for (const Line& line : lines) {
    switch (mAlign) {
    case Alignment::Left: baselineOffset[0] = -halfWidth; break;
    case Alignment::Center: baselineOffset[0] = -line.mWidth / 2.0f; break;
    case Alignment::Right: baselineOffset[0] = halfWidth - line.mWidth;
    }
    for (size_t i = line.mStart; i < line.mEnd; ++i) {
      Gfx::Renderable::Floater floater;
      floater.mOwner = owner.mMemberId;
      floater.mMeshId = Gfx::Renderer::nSpriteMeshId;
      floater.mMaterialId = mMaterialId;

      // Find the transformation of the renderable.
      // Find the scale that maintains the character's aspect ratio.
      const Gfx::Font::GlyphData& glyphData = font->GetGlyphData(mText[i]);
      Vec2 diagonal = glyphData.mTrOffset - glyphData.mBlOffset;
      Mat4 scale;
      Math::Scale(&scale, {diagonal[0], diagonal[1], 1.0f});
      // Find the translation offset from the text's relative origin.
      Vec2 centerOffset = (glyphData.mBlOffset + glyphData.mTrOffset) / 2.0f;
      Vec2 fullOffset = baselineOffset + centerOffset;
      Mat4 translate;
      Math::Translate(&translate, {fullOffset[0], fullOffset[1], 0.0f});
      floater.mTransform = ownerTransformation * translate * scale;

      // Add the text uniforms to the character renderable.
      GLuint& textureIdUniform = floater.mUniforms.Add<GLuint>(
        Gfx::UniformTypeId::Texture2d, "uTexture");
      textureIdUniform = font->GetTextureId(mText[i]);
      floater.mUniforms.Add<Vec4>("uColor") = mColor;
      floater.mUniforms.Add<float>("uFillAmount") = mFillAmount;

      Gfx::Collection::Add(std::move(floater));
      baselineOffset[0] += glyphData.mAdvance;
    }
    baselineOffset[1] -= font->NewlineOffset();
  }
}

void Text::VEdit(const World::Object& owner)
{
  Editor::DropResourceIdWidget(Rsl::ResTypeId::Font, &mFontId);
  Editor::DropResourceIdWidget(Rsl::ResTypeId::Material, &mMaterialId);
  ImGui::PushItemWidth(-Editor::CalcBufferWidth("FillAmount"));
  ImGui::DragFloat("Width", &mWidth, 1.0f, 0.0f, FLT_MAX);
  const char* alignments[] = {"Left", "Center", "Right"};
  int alignment = (int)mAlign;
  ImGui::Combo("Align", &alignment, alignments, 3);
  mAlign = (Comp::Text::Alignment)alignment;
  ImGui::DragFloat("FillAmount", &mFillAmount, 0.01f, 0.0f, 1.0f);
  ImGui::ColorEdit4("Color", mColor.mD);
  ImGui::PopItemWidth();
  ImGui::PushID(0);
  Editor::InputTextMultiline("", {-1.0f, 100.0f}, &mText);
  ImGui::PopID();
}

Ds::Vector<Text::Line> Text::GetLines(const Gfx::Font& font) const
{
  Ds::Vector<Line> lines;

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
  for (int i = 0; i < mText.size(); ++i) {
    // Find if we go into or out of a word.
    char codepoint = mText[i];
    if (!isWhitespace(codepoint) && wordStart == invalidLoc) {
      wordStart = i;
    }
    if (isWhitespace(codepoint) && wordStart != invalidLoc) {
      prevWordEnd = i;
      ++lineWordCount;
      wordStart = invalidLoc;
    }

    // End the line if a newline is encountered or if the line's width has
    // exceeded the max and it has at least one word.
    if (codepoint == '\n') {
      if (lineWordCount >= 1) {
        lineEnd = prevWordEnd;
      }
      else {
        lineEnd = lineStart;
      }
    }
    const Gfx::Font::GlyphData& glyphData = font.GetGlyphData(codepoint);
    currentWidth += glyphData.mAdvance;
    if (currentWidth > mWidth && lineWordCount >= 1) {
      lineEnd = prevWordEnd;
    }

    // If the end has been chosen, add the line and prepare for the next.
    if (lineEnd != invalidLoc) {
      lines.Push({lineStart, lineEnd});
      if (lineEnd == lineStart) {
        lineStart = i + 1;
      }
      else {
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
  for (Line& line : lines) {
    line.mWidth = 0.0f;
    for (size_t i = line.mStart; i < line.mEnd; ++i) {
      const Gfx::Font::GlyphData& glyphData = font.GetGlyphData(mText[i]);
      line.mWidth += glyphData.mAdvance;
    }
  }
  return lines;
}

std::ostream& operator<<(std::ostream& os, const Text::Alignment& align)
{
  switch (align) {
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
  if (alignString == "Center") {
    align = Text::Alignment::Center;
  }
  else if (alignString == "Right") {
    align = Text::Alignment::Right;
  }
  return is;
}

} // namespace Comp
