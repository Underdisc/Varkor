#ifndef ShadowMap_h
#define ShadowMap_h

#include <glad/glad.h>

#include "math/Matrix4.h"
#include "rsl/ResourceId.h"
#include "vlk/Valkor.h"
#include "world/Object.h"

namespace Comp {

struct ShadowMap
{
  GLuint mFbo;
  GLuint mTbo;
  unsigned int mWidth;
  unsigned int mHeight;
  float mBias;

  constexpr static unsigned int smDefaultWidth = 1024;
  constexpr static unsigned int smDefaultHeight = 1024;
  constexpr static float smDefaultBias = 0.0f;

  ~ShadowMap();

  void VInit(const World::Object& owner);
  void VSerialize(Vlk::Value& val);
  void VDeserialize(const Vlk::Explorer& ex);
  void VEdit(const World::Object& owner);
  Mat4 ProjView(const World::Object& owner) const;

private:
  void CreateFramebuffer();
  void DeleteFramebuffer();
};

} // namespace Comp

#endif