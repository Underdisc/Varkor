#ifndef comp_SpotLight_h
#define comp_SpotLight_h

#include "math/Vector.h"

namespace Comp {

#pragma pack(push, 1)
struct SpotLight
{
  SpotLight();

  void SetInnerCutoff(float angle);
  void SetOuterCutoff(float angle);
  float GetInnerCutoff() const;
  float GetOuterCutoff() const;

  void EditorHook();

  Vec3 mPosition;
  Vec3 mDirection;

  Vec3 mAmbient;
  Vec3 mDiffuse;
  Vec3 mSpecular;

  float mConstant;
  float mLinear;
  float mQuadratic;

  float mInnerCutoff;
  float mOuterCutoff;
};
#pragma pack(pop)

} // namespace Comp

#endif
