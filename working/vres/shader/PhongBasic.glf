#type fragment

#include "vres/shader/uniformBuffers/Lights.glh"
#include "vres/shader/uniformBuffers/Universal.glh"

in vec3 iNormal;
in vec3 iFragPos;

out vec4 iFinalColor;

void main()
{
  SurfaceData surface;
  surface.normal = normalize(iNormal);
  surface.diffuse = vec3(1.0, 1.0, 1.0);
  surface.specular = vec3(1.0, 1.0, 1.0);
  surface.exponent = 70.0;

  // Sum the contributions from all of the light types.
  vec3 viewDir = normalize(uViewPos - iFragPos);
  vec3 fragColor = vec3(0.0, 0.0, 0.0);
  for (uint i = uint(0); i < uDirectionalLightCount; ++i) {
    fragColor += CalcDirLight(uDirectionalLights[i], surface, viewDir);
  }
  for (uint i = uint(0); i < uPointLightCount; ++i) {
    fragColor += CalcPointLight(uPointLights[i], surface, viewDir, iFragPos);
  }
  for (uint i = uint(0); i < uSpotLightCount; ++i) {
    fragColor += CalcSpotLight(uSpotLights[i], surface, viewDir, iFragPos);
  }
  iFinalColor = vec4(fragColor, 1.0);
}