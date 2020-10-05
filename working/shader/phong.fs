#version 330 core

in vec3 normal;
in vec3 fragPos;

out vec4 finalColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float ambientStrength;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float specularStrength;
uniform float specularExponent;

void main()
{
  vec3 ambient = ambientStrength * lightColor;

  vec3 norm = normalize(normal);
  vec3 lightDir = normalize(lightPos - fragPos);
  float diffuseFactor = max(dot(lightDir, norm), 0.0);
  vec3 diffuse = diffuseFactor * lightColor;

  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float specularFactor =
    pow(max(dot(viewDir, reflectDir), 0.0), specularExponent);
  vec3 specular = specularStrength * specularFactor * lightColor;

  finalColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
