#ifndef gfx_Font_h
#define gfx_Font_h

#include <glad/glad.h>
#include <stb_truetype.h>

#include "Result.h"
#include "ds/Vector.h"
#include "math/Vector.h"
#include "vlk/Valkor.h"

namespace Gfx {

struct Font
{
public:
  struct InitInfo
  {
    std::string mFile;
    void Prep(const std::string& file);
    void Serialize(Vlk::Value& val) const;
    void Deserialize(const Vlk::Explorer& ex);
  };
  Result Init(const InitInfo& info);
  void Finalize() {};
  void Purge();

  Font();
  Font(Font&& other);
  Font& operator=(Font&& other);
  ~Font();

  Result Init(const std::string& file);

  struct GlyphMetrics
  {
    Vec2 mStartOffset, mEndOffset;
    float mAdvance;
  };
  const GlyphMetrics& GetGlyphMetrics(int codepoint) const;
  float NewlineOffset() const;
  GLuint GetTextureId(int codepoint);

private:
  void InitGlyphs();
  static constexpr size_t smGlyphCount = 256;

  stbtt_fontinfo mFontInfo;
  float mNewlineOffset;
  GlyphMetrics mAllGlyphMetrics[smGlyphCount];
  GLuint mTextureIds[smGlyphCount];
};

} // namespace Gfx

#endif
