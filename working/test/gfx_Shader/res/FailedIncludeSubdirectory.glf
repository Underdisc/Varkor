#type fragment

#include "fis0/FailedIncludeSubdirectory0.glh"

out vec4 finalColor;

void main()
{
  vec3 color = FailedIncludeSubdirectory1();
  finalColor = vec4(color, 1.0);
}
