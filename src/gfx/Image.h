#ifndef gfx_Image_h
#define gfx_Image_h

#include <glad/glad.h>

#include "util/Utility.h"

namespace Gfx {

struct Image
{
public:
  static constexpr size_t smInitPathCount = 1;
  static constexpr const char* smPathNames[smInitPathCount] = {"Image"};
  Util::Result Init(std::string paths[smInitPathCount]);
  void Purge();
  bool Live() const;

  Image();
  ~Image();
  Util::Result Init(const std::string& file);
  GLuint Id() const;
  float Aspect() const;

private:
  int mWidth;
  int mHeight;
  GLuint mId;
};

} // namespace Gfx

#endif
