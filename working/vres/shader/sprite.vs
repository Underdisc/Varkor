#include "uniformBuffers/matrices.glsl"

layout(location = 0) in vec2 aPos;
out vec2 iTextureCoords;

uniform mat4 uModel;

void main()
{
  gl_Position = uProj * uView * uModel * vec4(aPos, 0.0, 1.0);
  iTextureCoords.x = aPos.x + 0.5;
  iTextureCoords.y = aPos.y + 0.5;
}

