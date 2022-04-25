#include "Phong.glsl"
#include "SurfaceData.glsl"

struct DirectionalLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

vec3 CalcDirLight(DirectionalLight light, SurfaceData surface, vec3 viewDir)
{
  vec3 lightDir = normalize(-light.direction);

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

  return ambient + diffuse + specular;
}
