#ifndef editor_hook_Transform_h
#define editor_hook_Transform_h

#include "comp/Transform.h"
#include "editor/HookInterface.h"
#include "gfx/Framebuffer.h"
#include "math/Geometry.h"

namespace Editor {

template<>
struct Hook<Comp::Transform>: public HookInterface
{
public:
  Hook();
  void Edit(const World::Object& object);

private:
  enum class Mode
  {
    Translate,
    Scale,
    Rotate
  };
  enum class ReferenceFrame
  {
    World,
    Parent,
    Relative,
  };

private:
  Mode mMode;
  ReferenceFrame mReferenceFrame;
  bool mSnapping;
  float mTranslateSnapInterval;
  float mScaleSnapInterval;
  float mRotateSnapInterval;
};

} // namespace Editor

#endif
