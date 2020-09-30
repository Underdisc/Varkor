#version 330 core

out vec4 finalColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
  finalColor = vec4(lightColor * objectColor, 1.0);
}
