#include "Phong.glsl"
#include "SurfaceData.glsl"

struct PointLight
{
  vec3 position; // 0
  vec3 ambient; // 16
  vec3 diffuse; // 32
  vec3 specular; // 48
  float constant; // 64
  float linear; // 68
  float quadratic; // 72
}; // 80

vec3 CalcPointLight(
  PointLight light, SurfaceData surface, vec3 viewDir, vec3 fragPos)
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

  return ambient + diffuse + specular;
}
