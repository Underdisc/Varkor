#version 330 core

in vec3 iNormal;
in vec2 iTexCoord;
in vec3 iFragPos;

out vec4 finalColor;

struct PointLight
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

struct DirectionalLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct SpotLight
{
  vec3 position;
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;

  float innerCutoff;
  float outerCutoff;
};

struct Material
{
  sampler2D diffuseMap;
  sampler2D specularMap;
  float specularExponent;
};

#define POINT_LIGHT_COUNT 4

uniform vec3 viewPos;

uniform PointLight pointLights[POINT_LIGHT_COUNT];
uniform SpotLight spotLight;
uniform DirectionalLight dirLight;

uniform Material material;

vec3 CalcDiffuse(
  vec3 normal, vec3 lightDir, vec3 diffuseColor, vec3 diffuseSample);

vec3 CalcSpecular(
  vec3 normal,
  vec3 lightDir,
  vec3 viewDir,
  vec3 specularColor,
  vec3 specularSample);

float CalcAttenuation(
  float dist, float constant, float linear, float quadratic);

vec3 CalcPointLight(
  PointLight light,
  vec3 normal,
  vec3 viewDir,
  vec3 fragPos,
  vec3 diffuseSample,
  vec3 specularSample);

vec3 CalcSpotLight(
  SpotLight light,
  vec3 normal,
  vec3 viewDir,
  vec3 fragPos,
  vec3 diffuseSample,
  vec3 specularSample);

vec3 CalcDirLight(
  DirectionalLight light,
  vec3 normal,
  vec3 viewDir,
  vec3 diffuseSample,
  vec3 specularSample);

void main()
{
  vec3 normal = normalize(iNormal);
  vec3 viewDir = normalize(viewPos - iFragPos);
  vec3 diffuseSample = vec3(texture(material.diffuseMap, iTexCoord));
  vec3 specularSample = vec3(texture(material.specularMap, iTexCoord));

  // Sum the contributions from all of the light types.
  vec3 fragColor = vec3(0.0, 0.0, 0.0);
  for(int i = 0; i < POINT_LIGHT_COUNT; ++i)
  {
    fragColor += CalcPointLight(
      pointLights[i], normal, viewDir, iFragPos, diffuseSample, specularSample);
  }
  fragColor += CalcSpotLight(
    spotLight, normal, viewDir, iFragPos, diffuseSample, specularSample);
  fragColor +=
    CalcDirLight(dirLight, normal, viewDir, diffuseSample, specularSample);
  finalColor = vec4(fragColor, 1.0);
}

vec3 CalcPointLight(
  PointLight light,
  vec3 normal,
  vec3 viewDir,
  vec3 fragPos,
  vec3 diffuseSample,
  vec3 specularSample)
{
  vec3 lightVector = light.position - fragPos;
  float lightDist = length(lightVector);
  vec3 lightDir = normalize(lightVector);

  vec3 ambient = light.ambient * diffuseSample;
  vec3 diffuse = CalcDiffuse(normal, lightDir, light.diffuse, diffuseSample);
  vec3 specular =
    CalcSpecular(normal, lightDir, viewDir, light.specular, specularSample);

  float attenuation =
    CalcAttenuation(lightDist, light.constant, light.linear, light.quadratic);
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return ambient + diffuse + specular;
}

vec3 CalcSpotLight(
  SpotLight light,
  vec3 normal,
  vec3 viewDir,
  vec3 fragPos,
  vec3 diffuseSample,
  vec3 specularSample)
{
  vec3 lightVector = light.position - fragPos;
  float lightDist = length(lightVector);
  vec3 lightDir = normalize(lightVector);

  vec3 ambient = light.ambient * diffuseSample;
  vec3 diffuse = CalcDiffuse(normal, lightDir, light.diffuse, diffuseSample);
  vec3 specular =
    CalcSpecular(normal, lightDir, viewDir, light.specular, specularSample);

  float attenuation =
    CalcAttenuation(lightDist, light.constant, light.linear, light.quadratic);
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  vec3 lightBack = normalize(-light.direction);
  float cosTheta = dot(lightBack, lightDir);
  float cutoffDiff = light.innerCutoff - light.outerCutoff;
  float intensity = (cosTheta - light.outerCutoff) / cutoffDiff;
  intensity = clamp(intensity, 0.0, 1.0);
  diffuse *= intensity;
  specular *= intensity;

  return ambient + diffuse + specular;
}

vec3 CalcDirLight(
  DirectionalLight light,
  vec3 normal,
  vec3 viewDir,
  vec3 diffuseSample,
  vec3 specularSample)
{
  vec3 lightDir = normalize(-light.direction);

  vec3 ambient = light.ambient * diffuseSample;
  vec3 diffuse = CalcDiffuse(normal, lightDir, light.diffuse, diffuseSample);
  vec3 specular =
    CalcSpecular(normal, lightDir, viewDir, light.specular, specularSample);

  return ambient + diffuse + specular;
}

vec3 CalcDiffuse(
  vec3 normal, vec3 lightDir, vec3 diffuseColor, vec3 diffuseSample)
{
  float diffuseFactor = max(dot(lightDir, normal), 0.0);
  return diffuseFactor * diffuseColor * diffuseSample;
}

vec3 CalcSpecular(
  vec3 normal,
  vec3 lightDir,
  vec3 viewDir,
  vec3 specularColor,
  vec3 specularSample)
{
  vec3 reflectDir = reflect(-lightDir, normal);
  float specularFactor = max(dot(viewDir, reflectDir), 0.0);
  specularFactor = pow(specularFactor, material.specularExponent);
  return specularFactor * specularColor * specularSample;
}

float CalcAttenuation(float dist, float constant, float linear, float quadratic)
{
  float linearTerm = linear * dist;
  float quadraticTerm = quadratic * dist * dist;
  return 1.0 / (constant + linearTerm + quadraticTerm);
}
