#version 330 core

out vec4 iFinalColor;

uniform vec3 uLightColor;

void main()
{
  iFinalColor = vec4(uLightColor, 1.0);
}
