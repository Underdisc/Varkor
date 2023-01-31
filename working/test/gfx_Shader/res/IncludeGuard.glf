#type fragment

#include "IncludeGuard0.glh"
#include "IncludeGuard1.glh"

out vec4 iFinalColor;

void main()
{
  vec3 finalColor = IncludeGuard0() + IncludeGuard1();
  iFinalColor = vec4(finalColor, 1.0);
}