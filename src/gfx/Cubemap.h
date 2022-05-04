#ifndef gfx_Cubemap_h
#define gfx_Cubemap_h

#include <glad/glad.h>
#include <string>

#include "vlk/Valkor.h"

namespace Gfx {

struct Cubemap
{
  struct InitInfo
  {
    constexpr static const char* smFileDescriptors[6] {
      "Right", "Left", "Top", "Bottom", "Back", "Front"};
    std::string mFiles[6];

    constexpr static const char* smFilterTypes[2] {"Nearest", "Linear"};
    GLint mFilter;

    InitInfo();
    void Serialize(Vlk::Value& val) const;
    void Deserialize(const Vlk::Explorer& ex);
  };

  Result Init(const InitInfo& info);
  void Finalize() {};
  void Purge();

  Cubemap();
  Cubemap(Cubemap&& other);
  Cubemap& operator=(Cubemap&& other);
  ~Cubemap();

  Result Init(const std::string& file);
  GLuint Id() const;

private:
  GLuint mId;
};

} // namespace Gfx

#endif