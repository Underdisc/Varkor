#include <fstream>
#include <sstream>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#undef STB_TRUETYPE_IMPLEMENTATION

#include "gfx/Font.h"

namespace Gfx {

Util::Result Font::Init(std::string paths[smInitPathCount])
{
  return Init(paths[0]);
}

void Font::Purge()
{
  if (mTtfBuffer == nullptr)
  {
    return;
  }
  for (const auto& glyphSetPair : mGlyphSets)
  {
    for (size_t i = 0; i < smGlyphCount; ++i)
    {
      glDeleteTextures(1, &glyphSetPair.mValue[i]);
    }
  }
  mGlyphSets.Clear();
  delete[] mTtfBuffer;
}

bool Font::Live() const
{
  return mTtfBuffer != nullptr;
}

Font::Font(): mTtfBuffer(nullptr) {}

Font::~Font()
{
  Purge();
}

Util::Result Font::Init(const std::string& file)
{
  // Read the font file into a buffer.
  std::ifstream stream;
  stream.open(file, std::ifstream::binary);
  if (!stream.is_open())
  {
    std::stringstream error;
    error << file << " could not be opened.";
    return Util::Result(error.str());
  }
  std::filebuf* fileBuffer = stream.rdbuf();
  size_t bufferSize = fileBuffer->pubseekoff(0, stream.end, stream.in);
  fileBuffer->pubseekpos(0, stream.in);
  mTtfBuffer = new char[bufferSize];
  fileBuffer->sgetn(mTtfBuffer, bufferSize);
  stream.close();

  // Initialize the font with the buffer.
  int initResult = stbtt_InitFont(&mFontInfo, (unsigned char*)mTtfBuffer, 0);
  if (initResult == 0)
  {
    std::stringstream error;
    error << "Font within " << file << " could not be initialized.";
    Purge();
    return Util::Result(error.str());
  }

  // Calculate the metrics for all Glyphs. To get the scale applied to the
  // metrics, we take an arbitrary character (capital 'U' in this case) and find
  // the scale that makes the height of the codepoint box 1.
  int x0, y0, x1, y1;
  stbtt_GetCodepointBox(&mFontInfo, 'U', &x0, &y0, &x1, &y1);
  float scale = 1.0f / (float)(y1 - y0);
  for (size_t i = 0; i < smGlyphCount; ++i)
  {
    GlyphMetrics& glyphMetrics = mAllGlyphMetrics[i];
    stbtt_GetCodepointBox(&mFontInfo, (int)i, &x0, &y0, &x1, &y1);
    glyphMetrics.mStartOffset = Vec2({(float)x0, (float)y0}) * scale;
    glyphMetrics.mEndOffset = Vec2({(float)x1, (float)y1}) * scale;
    int advance;
    stbtt_GetCodepointHMetrics(&mFontInfo, (int)i, &advance, 0);
    glyphMetrics.mAdvance = advance * scale;
  }
  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&mFontInfo, &ascent, &descent, &lineGap);
  mNewlineOffset = (float)(ascent - descent + lineGap) * scale;
  return Util::Result(true);
}

const Font::GlyphBitmapIds& Font::InitBitmapIds(int pixelHeight)
{
  // Create and upload the bitmaps for the desired set of glpyhs.
  GlyphBitmapIds& bitmapIds = mGlyphSets.Emplace(pixelHeight);
  float renderScale = stbtt_ScaleForPixelHeight(&mFontInfo, (float)pixelHeight);
  for (size_t i = 0; i < smGlyphCount; ++i)
  {
    int glyphIndex = stbtt_FindGlyphIndex(&mFontInfo, (int)i);
    if (glyphIndex == 0)
    {
      bitmapIds[i] = 0;
      continue;
    }
    int width, height;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
      &mFontInfo, 0, renderScale, (int)i, &width, &height, 0, 0);
    glGenTextures(1, &bitmapIds[i]);
    glBindTexture(GL_TEXTURE_2D, bitmapIds[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_R8,
      width,
      height,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      bitmap);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbtt_FreeBitmap(bitmap, nullptr);
  }
  return bitmapIds;
}

const Font::GlyphBitmapIds& Font::GetBitmapIds(int pixelHeight)
{
  const GlyphBitmapIds* foundSet = mGlyphSets.Find(pixelHeight);
  if (foundSet != nullptr)
  {
    return *foundSet;
  }
  return InitBitmapIds(pixelHeight);
}

const Font::GlyphMetrics& Font::GetGlyphMetrics(int codepoint) const
{
  return mAllGlyphMetrics[codepoint];
}

float Font::NewlineOffset() const
{
  return mNewlineOffset;
}

} // namespace Gfx
