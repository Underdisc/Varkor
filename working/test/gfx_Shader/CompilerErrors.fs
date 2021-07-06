#version 330 core

#include "CompilerErrors0.glsl"

out vec4 finalColor;

void main()
{
  vec3 color = CompilerErrors2();
  finalColor = vec4(color, 1.0);
  error;
}
