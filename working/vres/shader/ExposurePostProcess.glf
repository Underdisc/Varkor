in vec2 iUv;
out vec4 iFinalColor;

uniform float uExposure;
uniform sampler2D uTexture;

void main()
{
  vec4 alphaColor = texture(uTexture, iUv);
  vec3 color = alphaColor.rgb;
  color = 1.0 - exp(-color * uExposure);
  iFinalColor = vec4(color, alphaColor.a);
}