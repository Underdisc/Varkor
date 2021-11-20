#ifndef comp_Text_h
#define comp_Text_h

#include <glad/glad.h>
#include <string>

#include "AssetLibrary.h"
#include "ds/Vector.h"
#include "math/Matrix4.h"
#include "vlk/Valkor.h"

namespace Comp {

#pragma pack(push, 1)
struct Text
{
  void VInit();
  void VSerialize(Vlk::Value& textVal);
  void VDeserialize(const Vlk::Explorer& textEx);

  struct Line
  {
    int mStart;
    int mEnd;
    float mWidth;
  };
  // Find the lines for a word wrapped version of the rendererd text.
  Ds::Vector<Line> GetLines() const;
  struct DrawInfo
  {
    GLuint mId;
    Mat4 mOffset;
  };
  // Get the DrawInfo for every character that needs to be rendered.
  Ds::Vector<DrawInfo> GetAllDrawInfo() const;

  AssetId mFontId;
  AssetId mShaderId;
  std::string mText;

  enum class Alignment
  {
    Left,
    Center,
    Right,
  };
  Alignment mAlign;
  float mWidth;
  Vec3 mColor;
};
#pragma pack(pop)

} // namespace Comp

#endif
