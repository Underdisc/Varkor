#ifndef comp_PointLight_h
#define comp_PointLight_h

#include "math/Vector.h"

namespace Comp {

#pragma pack(push, 1)
struct PointLight
{
  Vec3 mPosition;
  Vec3 mAmbient;
  Vec3 mDiffuse;
  Vec3 mSpecular;

  void EditorHook();
};
#pragma pack(pop)

} // namespace Comp

#endif
