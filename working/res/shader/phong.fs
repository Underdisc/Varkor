#include "vres/shader/lib/Phong.glsl"
#include "vres/shader/uniformBuffers/Lights.glsl"

in vec3 iFragPos;
in vec3 iNormal;
in vec2 iTexCoord;

out vec4 iFinalColor;

#define DIFFUSE_MAP_COUNT 3
#define SPECULAR_MAP_COUNT 2

struct Material
{
  sampler2D mDiffuse[DIFFUSE_MAP_COUNT];
  sampler2D mSpecular[SPECULAR_MAP_COUNT];
  float specularExponent;
};

#define POINT_LIGHT_COUNT 4

uniform vec3 viewPos;

uniform PointLight pointLights[POINT_LIGHT_COUNT];
uniform SpotLight spotLight;

uniform Material uMaterial;

void main()
{
  vec3 viewDir = normalize(viewPos - iFragPos);
  SurfaceData surface;
  surface.normal = normalize(iNormal);
  surface.diffuse = vec3(texture(uMaterial.mDiffuse[0], iTexCoord));
  surface.specular = vec3(texture(uMaterial.mSpecular[0], iTexCoord));
  surface.exponent = 16.0;

  // Sum the contributions from all of the light types.
  vec3 fragColor = vec3(0.0, 0.0, 0.0);
  // for(int i = 0; i < POINT_LIGHT_COUNT; ++i)
  //{
  //  fragColor += CalcPointLight(pointLights[i], surface, viewDir, iFragPos);
  //}
  // fragColor += CalcSpotLight(spotLight, surface, viewDir, iFragPos);
  fragColor += CalcDirLight(uDirectionalLight, surface, viewDir);
  iFinalColor = vec4(fragColor, 1.0);
}
