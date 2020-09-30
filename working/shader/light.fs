#version 330 core

out vec4 finalColor;

uniform vec3 lightColor;

void main()
{
  finalColor = vec4(lightColor, 1.0);
}
