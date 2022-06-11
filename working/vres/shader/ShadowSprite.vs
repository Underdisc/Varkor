#include "uniformBuffers/Universal.glsl"

layout(location = 0) in vec2 aPos;

out vec4 iShadowClipPos;
out vec2 iTextureCoords;

uniform mat4 uModel;
uniform mat4 uShadowProjView;

void main()
{
  vec4 worldPos = uModel * vec4(aPos, 0.0, 1.0);
  gl_Position = uProj * uView * worldPos;
  iShadowClipPos = uShadowProjView * worldPos;
  iTextureCoords.x = aPos.x + 0.5;
  iTextureCoords.y = aPos.y + 0.5;
}
