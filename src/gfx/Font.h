#ifndef gfx_Font_h
#define gfx_Font_h

#include <glad/glad.h>
#include <stb_truetype.h>

#include "ds/Map.h"
#include "math/Vector.h"
#include "util/Utility.h"

namespace Gfx {

struct Font
{
public:
  static constexpr size_t smInitPathCount = 1;
  static constexpr const char* smPathNames[smInitPathCount] = {"Font"};
  Util::Result Init(std::string paths[smInitPathCount]);
  void Purge();
  bool Live() const;

  Font();
  ~Font();
  Util::Result Init(const std::string& file);

  static constexpr size_t smGlyphCount = 256;
  typedef GLuint GlyphBitmapIds[smGlyphCount];
  const GlyphBitmapIds& InitBitmapIds(int pixelHeight);
  const GlyphBitmapIds& GetBitmapIds(int pixelHeight);

  struct GlyphMetrics
  {
    Vec2 mStartOffset, mEndOffset;
    float mAdvance;
  };
  const GlyphMetrics& GetGlyphMetrics(int codepoint) const;
  float NewlineOffset() const;

private:
  char* mTtfBuffer;
  stbtt_fontinfo mFontInfo;
  Ds::Map<int, GlyphBitmapIds> mGlyphSets;
  GlyphMetrics mAllGlyphMetrics[smGlyphCount];
  float mNewlineOffset;
};

} // namespace Gfx

#endif
