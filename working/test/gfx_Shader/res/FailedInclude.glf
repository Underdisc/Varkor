#type fragment

#include "FailedInclude0.glh"

out vec4 finalColor;

void main()
{
  vec3 color = FailedInclude0();
  finalColor = vec4(color, 1.0);
}
