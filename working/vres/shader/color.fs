#version 330 core

out vec4 iFinalColor;

uniform vec4 uAlphaColor;

void main()
{
  iFinalColor = uAlphaColor;
}
