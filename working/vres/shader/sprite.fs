#version 330 core

out vec4 iFinalColor;
in vec2 iTextureCoords;

uniform sampler2D uTexture;

void main()
{
  iFinalColor = texture(uTexture, iTextureCoords);
}
