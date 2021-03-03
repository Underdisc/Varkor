#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 iNormal;
out vec2 iTexCoord;
out vec3 iFragPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main()
{
  gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
  iNormal = mat3(uModel) * aNormal;
  iTexCoord = aTexCoord;
  iFragPos = vec3(uModel * vec4(aPos, 1.0));
}
