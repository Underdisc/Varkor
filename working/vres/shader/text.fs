#version 330 core

#include "text.glsl"

in vec2 iTextureCoords;
out vec4 iFinalColor;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform float uFillAmount;

void main()
{
  float value = texture(uTexture, iTextureCoords).r;
  float edgeValue = onEdgeValue + (1.0 - uFillAmount) * (1.0 - onEdgeValue);
  iFinalColor = BasicText(value, uColor, edgeValue);
}
