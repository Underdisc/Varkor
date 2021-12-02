#ifndef gfx_Image_h
#define gfx_Image_h

#include <glad/glad.h>

#include "Result.h"

namespace Gfx {

struct Image
{
public:
  static constexpr size_t smInitPathCount = 1;
  static constexpr const char* smPathNames[smInitPathCount] = {"Image"};
  Result Init(std::string paths[smInitPathCount]);
  void Purge();
  bool Live() const;

  Image();
  Image(Image&& other);
  Image& operator=(Image&& other);
  ~Image();

  Result Init(const std::string& file);
  GLuint Id() const;
  float Aspect() const;

private:
  int mWidth;
  int mHeight;
  GLuint mId;
};

} // namespace Gfx

#endif
