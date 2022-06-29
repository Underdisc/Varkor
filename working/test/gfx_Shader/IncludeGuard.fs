#type fragment

#include "IncludeGuard0.glsl"
#include "IncludeGuard1.glsl"

out vec4 iFinalColor;

void main()
{
  vec3 finalColor = IncludeGuard0() + IncludeGuard1();
  iFinalColor = vec4(finalColor, 1.0);
}