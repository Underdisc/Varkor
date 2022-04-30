#ifndef gfx_Image_h
#define gfx_Image_h

#include <glad/glad.h>

#include "Result.h"
#include "ds/Vector.h"
#include "vlk/Valkor.h"

namespace Gfx {

struct Image
{
public:
  struct InitInfo
  {
    std::string mFile;
    void Serialize(Vlk::Value& val) const;
    void Deserialize(const Vlk::Explorer& ex);
  };
  Result Init(const InitInfo& info);
  void Finalize() {};
  void Purge();

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
