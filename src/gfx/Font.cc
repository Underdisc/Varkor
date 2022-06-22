#include <fstream>
#include <sstream>
#include <utility>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#undef STB_TRUETYPE_IMPLEMENTATION

#include "AssetLibrary.h"
#include "gfx/Font.h"

namespace Gfx {

void Font::InitInfo::Prep(const std::string& file)
{
  mFile = file;
}

void Font::InitInfo::Serialize(Vlk::Value& val) const
{
  val("File") = mFile;
}

void Font::InitInfo::Deserialize(const Vlk::Explorer& ex)
{
  mFile = ex("File").As<std::string>("");
}

void Font::Purge()
{
  if (mFontInfo.data == nullptr) {
    return;
  }
  delete[] mFontInfo.data;
  mFontInfo.data = nullptr;
  for (size_t i = 0; i < smGlyphCount; ++i) {
    glDeleteTextures(1, &mTextureIds[i]);
  }
}

Font::Font()
{
  mFontInfo.data = nullptr;
}

Font::Font(Font&& other)
{
  *this = std::forward<Font>(other);
}

Font& Font::operator=(Font&& other)
{
  mFontInfo.data = other.mFontInfo.data;
  mNewlineOffset = other.mNewlineOffset;
  for (size_t i = 0; i < smGlyphCount; ++i) {
    mAllGlyphMetrics[i] = other.mAllGlyphMetrics[i];
    mTextureIds[i] = other.mTextureIds[i];
  }

  other.mFontInfo.data = nullptr;

  return *this;
}

Font::~Font()
{
  Purge();
}

Result Font::Init(const InitInfo& info)
{
  // Resolve the resource path.
  ValueResult<std::string> resolutionResult =
    AssLib::ResolveResourcePath(info.mFile);
  if (!resolutionResult.Success()) {
    return Result(resolutionResult.mError);
  }
  std::string path = resolutionResult.mValue;

  // Read the font file into a buffer.
  std::ifstream stream;
  stream.open(path, std::ifstream::binary);
  if (!stream.is_open()) {
    std::stringstream error;
    error << path << " could not be opened.";
    return Result(error.str());
  }
  std::filebuf* fileBuffer = stream.rdbuf();
  size_t bufferSize = fileBuffer->pubseekoff(0, stream.end, stream.in);
  fileBuffer->pubseekpos(0, stream.in);
  char* data = new char[bufferSize];
  fileBuffer->sgetn(data, bufferSize);
  stream.close();

  // Initialize the font with the buffer and initialize the glyphs.
  int initResult = stbtt_InitFont(&mFontInfo, (unsigned char*)data, 0);
  if (initResult == 0) {
    std::stringstream error;
    error << "Font within " << path << " could not be initialized.";
    Purge();
    return Result(error.str());
  }
  InitGlyphs();
  return Result();
}

Result Font::Init(const std::string& file)
{
  InitInfo info;
  info.mFile = file;
  return Init(info);
}

void Font::InitGlyphs()
{
  // To get the scale applied to the metrics for baseline offsets, advances,
  // line gap, etc. we take an arbitrary character (capital 'U' in this case)
  // and find the scale that makes the height of its box 1.
  int x0, y0, x1, y1;
  stbtt_GetCodepointBox(&mFontInfo, 'U', &x0, &y0, &x1, &y1);
  float toPreWorldScale = 1.0f / (float)(y1 - y0);
  int ascent, descent, lineGap;
  stbtt_GetFontVMetrics(&mFontInfo, &ascent, &descent, &lineGap);
  mNewlineOffset = (float)(ascent - descent + lineGap) * toPreWorldScale;

  const int pixelHeight = 64;
  const float renderScale =
    stbtt_ScaleForPixelHeight(&mFontInfo, (float)pixelHeight);
  for (size_t i = 0; i < smGlyphCount; ++i) {
    const int padding = 4;
    const int onEdgeValue = 120;
    const float pixelDistance = 40.0f;
    int textureWidth, textureHeight;
    int glyphIndex = stbtt_FindGlyphIndex(&mFontInfo, (int)i);
    unsigned char* textureData = stbtt_GetGlyphSDF(
      &mFontInfo,
      renderScale,
      glyphIndex,
      padding,
      onEdgeValue,
      pixelDistance,
      &textureWidth,
      &textureHeight,
      nullptr,
      nullptr);

    if (textureData != nullptr) {
      // Upload the glyph sdf texture.
      glGenTextures(1, &mTextureIds[i]);
      glBindTexture(GL_TEXTURE_2D, mTextureIds[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,
        textureWidth,
        textureHeight,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        textureData);
      glGenerateMipmap(GL_TEXTURE_2D);
      stbtt_FreeBitmap(textureData, nullptr);
    }
    else {
      mTextureIds[i] = 0;
    }

    // Calculate the glyph's metrics.
    GlyphMetrics& glyphMetrics = mAllGlyphMetrics[i];
    stbtt_GetCodepointBox(&mFontInfo, (int)i, &x0, &y0, &x1, &y1);
    Vec2 boxStart = Vec2({(float)x0, (float)y0}) * toPreWorldScale;
    Vec2 boxEnd = Vec2({(float)x1, (float)y1}) * toPreWorldScale;
    // We find the percentage of the texture's height and width that a single
    // pixel takes when unpadded. This allows us to add padding to the box using
    // the size of a pixel within it.
    int fullPadding = 2 * padding;
    float pixelHeightRatio = 1.0f / (float)(textureHeight - fullPadding);
    float pixelWidthRatio = 1.0f / (float)(textureWidth - fullPadding);
    float boxWidth = boxEnd[0] - boxStart[0];
    float boxHeight = boxEnd[1] - boxStart[1];
    float pixelWidth = pixelWidthRatio * boxWidth;
    float pixelHeight = pixelHeightRatio * boxHeight;
    GlyphMetrics& glyphM = mAllGlyphMetrics[i];
    glyphM.mStartOffset[0] = boxStart[0] - pixelWidth * padding;
    glyphM.mStartOffset[1] = boxStart[1] - pixelHeight * padding;
    glyphM.mEndOffset[0] = boxEnd[0] + pixelWidth * padding;
    glyphM.mEndOffset[1] = boxEnd[1] + pixelHeight * padding;
    int advance;
    stbtt_GetCodepointHMetrics(&mFontInfo, (int)i, &advance, 0);
    glyphMetrics.mAdvance = advance * toPreWorldScale;
  }
  glFinish();
}

GLuint Font::GetTextureId(int codepoint)
{
  return mTextureIds[codepoint];
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
