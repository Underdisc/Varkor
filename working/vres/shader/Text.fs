#version 330 core

in vec2 iTextureCoords;
out vec4 iFinalColor;

uniform sampler2D uTexture;

void main()
{
  float red = texture(uTexture, iTextureCoords).r;
  const float discardThreshold = 0.1;
  if (red < discardThreshold)
  {
    discard;
  }
  iFinalColor = vec4(red, red, red, red);
}
