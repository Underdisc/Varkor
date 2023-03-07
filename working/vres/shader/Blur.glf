#type fragment

in vec2 iUv;
out vec4 iFinalColor;

uniform vec2 uDirection;
uniform sampler2D uTexture;

void main()
{
  vec2 texelSize = 1 / vec2(textureSize(uTexture, 0));
  const float weightCount = 15;
  const float weights[15] = float[](
    0.03746, 0.03734, 0.03695, 0.03632, 0.03546, 0.03438, 0.03311, 0.03166,
    0.03007, 0.02837, 0.02657, 0.02473, 0.02285, 0.02097, 0.01911);
  iFinalColor += texture(uTexture, iUv) * weights[0];
  for (int i = 1; i < weightCount; ++i) {
    vec2 uvOffset = uDirection * texelSize * i;
    iFinalColor += texture(uTexture, iUv + uvOffset) * weights[i];
    iFinalColor += texture(uTexture, iUv - uvOffset) * weights[i];
  }
}