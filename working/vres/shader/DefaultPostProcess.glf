#type fragment

in vec2 iUv;
out vec4 iFinalColor;

uniform sampler2D uTexture;

void main()
{
  iFinalColor = texture(uTexture, iUv);
}