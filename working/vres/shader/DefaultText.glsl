#type vertex

#include "uniformBuffers/Universal.glsl"

layout(location = 0) in vec2 aPos;
out vec2 iTextureCoords;

uniform mat4 uModel;

void main()
{
  gl_Position = uProj * uView * uModel * vec4(aPos, 0.0, 1.0);
  iTextureCoords.x = aPos.x + 0.5;
  iTextureCoords.y = 1.0f - (aPos.y + 0.5);
}

////////////////////////////////////////////////////////////////////////////////
#type fragment

#include "lib/Text.glsl"

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