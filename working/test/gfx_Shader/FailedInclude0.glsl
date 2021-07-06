#include "FailedInclude1.glsl"
vec3 FailedInclude0
{
  return vec3(1.0, 1.0, 1.0);
}
#include "notafile"
#include "FailedInclude2.glsl"
