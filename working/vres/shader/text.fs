#version 330 core

#include "text.glsl"

in vec2 iTextureCoords;
out vec4 iFinalColor;

uniform sampler2D uTexture;
uniform vec3 uColor;

void main()
{
  float value = texture(uTexture, iTextureCoords).r;
  iFinalColor = BasicText(value, uColor);
}
