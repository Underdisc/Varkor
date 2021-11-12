#version 330 core

layout(location = 0) in vec2 aPos;
out vec2 iTextureCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main()
{
  gl_Position = uProj * uView * uModel * vec4(aPos, 0.0, 1.0);
  iTextureCoords.x = aPos.x + 0.5;
  iTextureCoords.y = 1.0f - (aPos.y + 0.5);
}
