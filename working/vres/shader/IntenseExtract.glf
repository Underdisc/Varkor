#type fragment

in vec2 iUv;
out vec4 iFinalColor;

uniform sampler2D uTexture;
uniform float uLuminanceThreshold;

void main()
{
  const vec3 luminanceWeights = vec3(0.2126, 0.7152, 0.0722);
  vec4 color = texture(uTexture, iUv);
  float luminance = dot(color.rgb, luminanceWeights);
  if (luminance > uLuminanceThreshold) {
    iFinalColor = vec4(color.rgb, 1.0);
    return;
  }
  iFinalColor = vec4(0, 0, 0, 1.0);
}
