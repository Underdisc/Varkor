#include "vres/shader/lib/Phong.glsl"
#include "vres/shader/lib/SurfaceData.glsl"

struct SpotLight
{
  vec3 position; // 0
  vec3 direction; // 16
  vec3 ambient; // 32
  vec3 diffuse; // 48
  vec3 specular; // 64
  float constant; // 76
  float linear; // 80
  float quadratic; // 84
  float innerCutoff; // 88
  float outerCutoff; // 92
}; // 96

vec3 CalcSpotLight(
  SpotLight light, SurfaceData surface, vec3 viewDir, vec3 fragPos)
{
  vec3 lightVector = light.position - fragPos;
  float lightDist = length(lightVector);
  vec3 lightDir = normalize(lightVector);

  vec3 ambient = light.ambient * surface.diffuse;
  vec3 diffuse =
    CalcDiffuse(surface.normal, lightDir, light.diffuse, surface.diffuse);
  vec3 specular = BlinnSpecular(
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
