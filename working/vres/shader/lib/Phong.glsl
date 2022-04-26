#include "SurfaceData.glsl"

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
  vec3 lightColor,
  vec3 sample,
  float exponent)
{
  vec3 reflectDir = reflect(-lightDir, normal);
  float factor = max(dot(viewDir, reflectDir), 0.0);
  factor = pow(factor, exponent);
  return factor * lightColor * sample;
}

float CalcAttenuation(float dist, float constant, float linear, float quadratic)
{
  float linearTerm = linear * dist;
  float quadraticTerm = quadratic * dist * dist;
  return 1.0 / (constant + linearTerm + quadraticTerm);
}

vec3 CalcSpotLight(
  SpotLight light, SurfaceData surface, vec3 viewDir, vec3 fragPos)
{
  vec3 lightVector = light.position - fragPos;
  float lightDist = length(lightVector);
  vec3 lightDir = normalize(lightVector);

  vec3 ambient = light.ambient * surface.diffuse;
  vec3 diffuse =
    CalcDiffuse(surface.normal, lightDir, light.diffuse, surface.diffuse);
  vec3 specular = CalcSpecular(
    surface.normal,
    lightDir,
    viewDir,
    light.specular,
    surface.specular,
    surface.exponent);

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
