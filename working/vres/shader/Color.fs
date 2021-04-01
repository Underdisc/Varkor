#version 330 core

out vec4 iFinalColor;

uniform vec3 uColor;

void main()
{
  iFinalColor = vec4(uColor, 1.0);
}
