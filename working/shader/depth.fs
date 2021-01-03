#version 330 core

out vec4 iFinalColor;

uniform float uNear;
uniform float uFar;

void main()
{
  // Find the z value in view space.
  float depth = gl_FragCoord.z * 2.0 - 1.0;
  depth = (2.0 * uNear * uFar) / (depth * (uFar - uNear) - uFar + uNear);
  // We negate depth because z values in view space are negative.
  depth = -depth / uFar;
  iFinalColor = vec4(vec3(depth), 1.0);
}
