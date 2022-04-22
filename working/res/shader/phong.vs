#include "vres/shader/uniformBuffers/matrices.glsl"

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 iNormal;
out vec2 iTexCoord;
out vec3 iFragPos;

uniform mat4 uModel;

void main()
{
  gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
  iFragPos = vec3(uModel * vec4(aPos, 1.0));
  iNormal = mat3(uModel) * aNormal;
  iTexCoord = aTexCoord;
}
