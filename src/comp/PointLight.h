#ifndef comp_PointLight_h
#define comp_PointLight_h

#include "math/Vector.h"

namespace Comp {

#pragma pack(push, 1)
struct PointLight
{
  PointLight();

  Vec3 mPosition;
  Vec3 mAmbient;
  Vec3 mDiffuse;
  Vec3 mSpecular;

  float mConstant;
  float mLinear;
  float mQuadratic;

  void EditorHook();
};
#pragma pack(pop)

} // namespace Comp

#endif
