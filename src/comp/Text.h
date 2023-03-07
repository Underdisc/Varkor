#ifndef comp_Text_h
#define comp_Text_h

#include <glad/glad.h>
#include <string>

#include "ds/Vector.h"
#include "gfx/Font.h"
#include "math/Matrix4.h"
#include "rsl/ResourceId.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

#pragma pack(push, 1)
struct Text
{
  constexpr static const char* smDefaultAssetName = "vres/text";
  static const ResId smDefaultFontId;
  static const ResId smDefaultMaterialId;

  void VStaticInit();
  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& textVal);
  void VDeserialize(const Vlk::Explorer& textEx);
  void VRenderable(const World::Object& owner);
  void VEdit(const World::Object& owner);

  struct Line
  {
    int mStart;
    int mEnd;
    float mWidth;
  };
  // Find the lines for a word wrapped version of the rendererd text.
  Ds::Vector<Line> GetLines(const Gfx::Font& font) const;

  ResId mFontId;
  ResId mMaterialId;
  std::string mText;

  enum class Alignment
  {
    Left,
    Center,
    Right,
  };
  Alignment mAlign;
  float mWidth;
  bool mVisible;
  float mFillAmount;
  Vec4 mColor;
};
#pragma pack(pop)

} // namespace Comp

#endif
