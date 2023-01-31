#type fragment

in vec2 iUv;
out vec4 iFinalColor;

uniform sampler2D uTexture;

void main()
{
  vec4 alphaColor = texture(uTexture, iUv);
  vec3 color = alphaColor.rgb;
  color = color / (color + 1.0);
  iFinalColor = vec4(color, alphaColor.a);
}