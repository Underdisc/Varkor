#version 330 core

in vec3 normal;
in vec2 texCoord;
in vec3 fragPos;

out vec4 finalColor;

struct Light
{
  vec4 vector;
  vec3 ambientColor;
  vec3 diffuseColor;
  vec3 specularColor;
};

struct Material
{
  sampler2D diffuseMap;
  sampler2D specularMap;
  float specularExponent;
};

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
  vec3 lightDir;
  if (light.vector.w == 0.0)
  {
    lightDir = normalize(-light.vector.xyz);
  }
  else if (light.vector.w == 1.0)
  {
    lightDir = normalize(light.vector.xyz - fragPos);
  }

  vec3 diffuseSample = vec3(texture(material.diffuseMap, texCoord));
  vec3 ambient = light.ambientColor * diffuseSample;

  vec3 norm = normalize(normal);
  float diffuseFactor = max(dot(lightDir, norm), 0.0);
  vec3 diffuse = diffuseFactor * light.diffuseColor * diffuseSample;

  vec3 viewDir = normalize(viewPos - fragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float specularFactor = max(dot(viewDir, reflectDir), 0.0);
  specularFactor = pow(specularFactor, material.specularExponent);
  vec3 specularSample = vec3(texture(material.specularMap, texCoord));
  vec3 specular = specularFactor * light.specularColor * specularSample;

  finalColor = vec4(ambient + diffuse + specular, 1.0);
}
