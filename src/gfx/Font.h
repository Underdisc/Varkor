#ifndef gfx_Font_h
#define gfx_Font_h

#include <glad/glad.h>
#include <stb_truetype.h>

#include "Result.h"
#include "gfx/Image.h"
#include "math/Vector.h"
#include "vlk/Valkor.h"

namespace Gfx {

struct Font {
public:
  Font();
  Font(Font&& other);
  Font& operator=(Font&& other);
  ~Font();

  static void EditConfig(Vlk::Value* configValP);
  Result Init(const Vlk::Explorer& configEx);
  Result Init(const std::string& file);

  struct GlyphData {
    // The bottom left and top right offsets from the center of the glyph.
    Vec2 mBlOffset;
    Vec2 mTrOffset;
    // The distance from the center of the glyph to the next.
    float mAdvance;
    Image mImage;
  };
  const GlyphData& GetGlyphData(int codepoint) const;
  float NewlineOffset() const;
  GLuint GetTextureId(int codepoint);

private:
  void InitGlyphs();
  static constexpr size_t smGlyphCount = 256;

  stbtt_fontinfo mFontInfo;
  float mNewlineOffset;
  GlyphData mGlyphData[smGlyphCount];
};

} // namespace Gfx

#endif
