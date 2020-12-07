#ifndef comp_DirectionalLight_h
#define comp_DirectionalLight_h

#include "math/Vector.h"

namespace Comp {

#pragma pack(push, 1)
struct DirectionalLight
{
  Vec3 mDirection;
  Vec3 mAmbient;
  Vec3 mDiffuse;
  Vec3 mSpecular;

  void EditorHook();
};
#pragma pack(pop)

} // namespace Comp

#endif
