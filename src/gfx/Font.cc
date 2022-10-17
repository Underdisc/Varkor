#include <fstream>
#include <imgui/imgui.h>
#include <utility>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#undef STB_TRUETYPE_IMPLEMENTATION

#include "editor/Utility.h"
#include "gfx/Font.h"
#include "rsl/Library.h"

namespace Gfx {

Font::Font()
{
  mFontInfo.data = nullptr;
}

Font::Font(Font&& other)
{
  *this = std::move(other);
}

Font& Font::operator=(Font&& other)
{
  mFontInfo.data = other.mFontInfo.data;
  mNewlineOffset = other.mNewlineOffset;
  for (size_t i = 0; i < smGlyphCount; ++i) {
    mGlyphData[i] = std::move(other.mGlyphData[i]);
  }

  other.mFontInfo.data = nullptr;

  return *this;
}

Font::~Font()
{
  if (mFontInfo.data == nullptr) {
    return;
  }
  delete[] mFontInfo.data;
}

void Font::EditConfig(Vlk::Value* configValP)
{
  Vlk::Value& configVal = *configValP;
  Vlk::Value& fileVal = configVal("File");
  std::string file = fileVal.As<std::string>("");
  // todo: drag and drop
  Editor::InputText("File", &file, -Editor::CalcBufferWidth("File"));
  fileVal = file;
}

Result Font::Init(const Vlk::Explorer& configEx)
{
  Vlk::Explorer fileEx = configEx("File");
  if (!fileEx.Valid(Vlk::Value::Type::TrueValue)) {
    return Result("Missing :File: TrueValue.");
  }
  return Init(fileEx.As<std::string>());
}

Result Font::Init(const std::string& file)
{
  // Resolve the resource path.
  ValueResult<std::string> resolutionResult = Rsl::ResolveResPath(file);
  if (!resolutionResult.Success()) {
    return Result(resolutionResult.mError);
  }
  const std::string& absoluteFile = resolutionResult.mValue;

  // Read the font file into a buffer.
  std::ifstream stream;
  stream.open(absoluteFile, std::ifstream::binary);
  if (!stream.is_open()) {
    return Result("Failed to open\"" + absoluteFile + "\".");
  }
  std::filebuf* fileBuffer = stream.rdbuf();
  size_t bufferSize = fileBuffer->pubseekoff(0, stream.end, stream.in);
  fileBuffer->pubseekpos(0, stream.in);
  char* data = alloc char[bufferSize];
  fileBuffer->sgetn(data, bufferSize);
  stream.close();

  // Initialize the font with the buffer and initialize the glyphs.
  int initResult = stbtt_InitFont(&mFontInfo, (unsigned char*)data, 0);
  if (initResult == 0) {
    delete[] mFontInfo.data;
    return Result("Failed to initialize \"" + absoluteFile + "\".");
  }
  InitGlyphs();
  return Result();
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
    GlyphData& glyphData = mGlyphData[i];
    const int padding = 4;
    const int onEdgeValue = 120;
    const float pixelDistance = 40.0f;
    int glyphIndex = stbtt_FindGlyphIndex(&mFontInfo, (int)i);
    Image::Config imageConfig;
    imageConfig.mData = stbtt_GetGlyphSDF(
      &mFontInfo,
      renderScale,
      glyphIndex,
      padding,
      onEdgeValue,
      pixelDistance,
      &imageConfig.mWidth,
      &imageConfig.mHeight,
      nullptr,
      nullptr);

    if (imageConfig.mData != nullptr) {
      imageConfig.mInternalFormat = GL_R8;
      imageConfig.mFormat = GL_RED;
      imageConfig.mPixelAlignment = 1;
      glyphData.mImage.Init(imageConfig);
      stbtt_FreeBitmap(imageConfig.mData, nullptr);
    }

    // Calculate the glyph's metrics.
    stbtt_GetCodepointBox(&mFontInfo, (int)i, &x0, &y0, &x1, &y1);
    Vec2 boxStart = Vec2({(float)x0, (float)y0}) * toPreWorldScale;
    Vec2 boxEnd = Vec2({(float)x1, (float)y1}) * toPreWorldScale;
    // We find the percentage of the texture's height and width that a single
    // pixel takes when unpadded. This allows us to add padding to the box using
    // the size of a pixel within it.
    int fullPadding = 2 * padding;
    float pixelHeightRatio = 1.0f / (float)(imageConfig.mHeight - fullPadding);
    float pixelWidthRatio = 1.0f / (float)(imageConfig.mWidth - fullPadding);
    float boxWidth = boxEnd[0] - boxStart[0];
    float boxHeight = boxEnd[1] - boxStart[1];
    float pixelWidth = pixelWidthRatio * boxWidth;
    float pixelHeight = pixelHeightRatio * boxHeight;
    glyphData.mBlOffset[0] = boxStart[0] - pixelWidth * padding;
    glyphData.mBlOffset[1] = boxStart[1] - pixelHeight * padding;
    glyphData.mTrOffset[0] = boxEnd[0] + pixelWidth * padding;
    glyphData.mTrOffset[1] = boxEnd[1] + pixelHeight * padding;
    int advance;
    stbtt_GetCodepointHMetrics(&mFontInfo, (int)i, &advance, 0);
    glyphData.mAdvance = (float)advance * toPreWorldScale;
  }
}

GLuint Font::GetTextureId(int codepoint)
{
  return mGlyphData[codepoint].mImage.Id();
}

const Font::GlyphData& Font::GetGlyphData(int codepoint) const
{
  return mGlyphData[codepoint];
}

float Font::NewlineOffset() const
{
  return mNewlineOffset;
}

} // namespace Gfx
