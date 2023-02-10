#type fragment

in vec2 iUv;
out vec4 iFinalColor;

uniform sampler2D uTexture;

void main()
{
  vec3 color = texture(uTexture, iUv).rgb;
  color = color / (color + 1.0);
  iFinalColor = vec4(color, 1.0);
}