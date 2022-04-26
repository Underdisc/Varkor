#include "vres/shader/lib/DirectionalLight.glsl"
#include "vres/shader/lib/PointLight.glsl"

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 100

layout(std140) uniform Lights
{
  uint uDirectionalLightCount; // 0
  uint uPointLightCount; // 4
  DirectionalLight uDirectionalLights[MAX_DIRECTIONAL_LIGHTS]; // 16
  PointLight uPointLights[MAX_POINT_LIGHTS]; // 80
}; // 8080