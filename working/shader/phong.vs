#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 iNormal;
out vec2 iTexCoord;
out vec3 iFragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
  gl_Position = proj * view * model * vec4(aPos, 1.0);
  iNormal = mat3(model) * aNormal;
  iTexCoord = aTexCoord;
  iFragPos = vec3(model * vec4(aPos, 1.0));
}
