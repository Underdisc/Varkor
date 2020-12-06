#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 normal;
out vec2 texCoord;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
  gl_Position = proj * view * model * vec4(aPos, 1.0);
  normal = mat3(model) * aNormal;
  texCoord = aTexCoord;
  fragPos = vec3(model * vec4(aPos, 1.0));
}
