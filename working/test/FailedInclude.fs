#version 330 core

#include "FailedInclude0.glsl"

out vec4 finalColor;

void main()
{
  vec3 color = FailedInclude0();
  finalColor = vec4(color, 1.0);
}
