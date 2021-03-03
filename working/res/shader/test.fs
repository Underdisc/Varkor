#version 330 core

in vec3 iNormal;
in vec2 iTexCoord;
in vec3 iFragPos;

out vec4 finalColor;

void main()
{
  vec3 normal = normalize(iFragPos);
  finalColor = vec4(normal, 1.0);
}
